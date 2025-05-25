# Python Global Interpreter Lock (GIL) deadlock bug breakdown - 25/05/2025

</br>
</br>

# Quick Summary
Any requests made to endpoints exposed via Drogon controllers were hanging indefinitely. 

Originally, in ```main.cpp``` within the ```main()``` function I was initialising **pybind11** and acquiring the Python GIL in order to run an initial test to some custom python code. This test call to the python code (via pybind11) in theory should have released the GIL after exiting its local block scope, but it wasn't.

Right after this blocked scope, Drogon was initialised and ran which itself blocks the thread running ```main()``` as Drogon essentially 'takes over' execution.

This meant the main thread never got to release the GIL. The Drogon workers that handled the HTTP requests were trying to call the custom python code, which required the GIL, which was never going to be released, ***resulting in a deadlock***.

The solution was to explicitly release the GIL in the main thread **after** the blocked scope exited, and **before** Drogon was initialised and ran. Additionally, the methods that call python code explicitly handle GIL acquiring and releasing via ```PyGILState_Ensure()``` and ```PyGILState_Release()``` respectively.

</br>
</br>

## The Original Issue
Any requests made to the ```TickerController``` (Drogon HTTP controller for /ticker, e.g. /ticker/msft) would hang indefinitely and even block SIGINT keyboard interrupt signals.

Under the hood, the TickerController was attempting to call a ```price_fetcher``` python function found in ```python/src/price_fetcher.py```, via the wrapper class ```YFinanceProvider.h```. This used **pybind11** to call the python module within a C++ application.

It was not clear what exactly was causing this indefinite hang. In fact, in ```main.cpp``` there was an initial test run which used this exact functionality before even starting the Drogon HTTP service, which ran without issue. 

After some (painfully long) investigation the issue seemed to stem from specifically when the TickerController would call ```YFinanceProvider::fetchHistory```, which in turn tried to call the python module for ```price_fetcher.py```. The **pybind11** call looks something like this:
```C++
py::module_ price_fetcher_module = py::module_::import("price_fetcher");
py::object result_obj = price_fetcher_module_.attr("fetch_history")( /*parameters*/ );
```

</br>
</br>
</br>


## What Was Causing The Deadlock
### Python's Global Interpreter Lock (GIL)
[Python uses a GIL](https://wiki.python.org/moin/GlobalInterpreterLock). Essentially in a single python process only **ONE** thread can be actively which means that at any given moment in a single Python process, only one thread can actively hold the mutex to the interpreter and thus execute Python bytecode. When C++ code (or anything really) wants to call Python code, it must first acquire this GIL.

---

### Initialising pybind11 in the main thread
In my ```main.cpp``` I initilise **pybind11** and explicitly enable Python threading support with:
```c++
py::scoped_interpreter guard{};
PyEval_InitThreads();
```
Then ***I run the initial test calls to ```price_fetcher.py```***. To do this, I was separating these calls in the ```main()``` function by placing all the related logic inside their own block scope (curly braces without a preceeding statement), and at the top of the nested block scope I was acquiring the GIL for this test. It looks something like this:
```c++
main()
{
    /* ...unrelated setup and initialisaitons... */

    {
        // grab GIL for 'this' thread
        py::gil_scoped_acquire acquire;
        /* run price_fetcher.py testing */
    }
/*  ^_____________________________________________________
    GIL acquired by py::gil_scoped_acquire SHOULD be FULLY 
        released here when the blocked scope exits
*/

    /* ...continue with other setup... */
}
```

Oddly however, the Python C-API ***still considered the main C++ thread as the primary owner of the GIL***, despite the blocked scope ending. Perhaps this is a result of unintended/undocumented edge cases with the Python C-API and the relatively complex mixture of frameworks/libraries this project uses, or (*most likely*) my own lack of deep familiarity with Python's C-API and usage of the Python GIL.

---

### Running the Drogon worker threads
***After*** running the initial ```price_fetcher.py``` tests, I was then initialising and running the Drogon main and IO event loops (essentially starting the Drogon server listeners). Under the hood, [Drogon spawns a main IO thread along with one or more worker threads](https://github.com/drogonframework/drogon/wiki/ENG-FAQ-1-Understanding-drogon-threading-model). When a HTTP request came in, Drogon dispatched it to one of its worker threads. This Drogon worker thread is a different C++ thread from the main application thread.

---

### Quick note: ```PyGILState_Ensure()```
The [```PyGilState_Ensure()```](https://docs.python.org/3/c-api/init.html#c.PyGILState_Ensure) function checks and ensures the current thread is ready to call the Python C-API. If the GIL is already held by 'something' else, **it will block and wait until the GIL can be acquired**. 

---

### The resulting deadlock
When this Drogon worker thread was trying to call Python bytecode via the ```YFinanceProvider::fetchHistory``` class, but it **first needs to acquire the GIL**, and in fact it *was* calling ```PyGILState_Ensure()``` to try and achieve this. However, ***the main thread was still implicitly holding onto the GIL*** as mentioned above. Specifically, ***the main thread itself was blocked*** when ```drogon::app().run()``` was called. This meant the call to ```PyGILState_Ensure()``` in the worker thread handling the HTTP request was waiting for a GIL that would never be released by the main thread block. Thus, a deadlock.


</br>
</br>
</br>

## How It Was Resolved

### Releasing the GIL in the main thread
Since the underlying issue was the main thread indefinitely holding onto the GIL, we needed to essentially release the GIL **before** the Drogon threads were launched, since Drogon would block the ```main.cpp``` thread and prevent releasing the GIL. By adding this call **BEFORE** running the Drogon app:

```c++
_save = PyEval_SaveThread();
```

We are explicitly telling Python to save the main thread's Python state and [release the GIL](https://docs.python.org/3/c-api/init.html#c.PyEval_SaveThread). The main thread is now 'paused' from Python's perspective, making the GIL available.

Now, when a Drogon worker thread calls ```PyGILState_Ensure()``` inside ```YFinanceProvider::fetchHistory```, it can successfully acquire the GIL since the ```main.cpp``` thread is not holding it indefinitely.

---

### Re-acquiring the GIL in the main thread
When ```drogon::app().run()``` eventually finishes and exits, we then call:

```c++
PyEval_RestoreThread(_save);
```
to allow the main thread to re-acquire the GIL. This is important for a clean shutdown, as the ```py::scoped_interpreter``` guard destructor will need the GIL to finalise.

---

### Local GIL management in Drogon worker threads 

The ```YFinanceProvider::fetchHistory``` method uses ```PyGILState_Ensure()``` at the beginning of its Python interaction, and subsequently ```PyGILState_Release()``` at the end, to ensure that each call from a Drogon worker thread is managing the GIL during its event handling/execution.
