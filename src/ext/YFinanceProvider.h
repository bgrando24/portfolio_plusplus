#ifndef YFINANCEPROVIDER_H
#define YFINANCEPROVIDER_H

#include <string>
#include <map>
#include <pybind11/embed.h> // Should be included by users of pybind11
#include <pybind11/stl.h>   // For automatic C++/Python STL conversions
#include <drogon/drogon.h>  // For LOG_DEBUG, LOG_ERROR etc.
#include "../util/Types.h"  // For Types::PriceHistory

namespace py = pybind11;

class YFinanceProvider
{
public:
    YFinanceProvider()
    {
        // This constructor is called from main.cpp where the GIL is already held
        // by the py::gil_scoped_acquire in main's scope.
        // So, we can safely import modules here.

        LOG_DEBUG << "YFinanceProvider constructor: Importing 'price_fetcher' module...";
        try
        {
            price_fetcher_module_ = py::module_::import("price_fetcher");
            LOG_INFO << "YFinanceProvider: 'price_fetcher' module imported successfully.";
        }
        catch (const py::error_already_set &e)
        {
            LOG_ERROR << "YFinanceProvider: Failed to import 'price_fetcher' module in constructor: " << e.what();
            if (PyErr_Occurred())
                PyErr_Print();
            // Consider re-throwing or setting an error state
        }
    }

    Types::PriceHistory fetchHistory(const std::string &symbol, const std::string &period)
    {
        LOG_DEBUG << "YFinanceProvider::fetchHistory called for symbol: " << symbol << ", period: " << period;

        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure(); // Acquire GIL for this Drogon worker thread

        Types::PriceHistory cpp_price_history;

        try
        {
            LOG_DEBUG << "YFinanceProvider: Calling Python 'fetch_history' for " << symbol;
            py::object result_obj = price_fetcher_module_.attr("fetch_history")(symbol, period);
            LOG_DEBUG << "YFinanceProvider: Python 'fetch_history' call returned for " << symbol;

            // IMPORTANT: The conversion logic below assumes price_fetcher.py returns a dict
            // compatible with Types::PriceHistory (std::map<std::string, std::map<std::string, double>>).
            // Your current price_fetcher.py returns a simpler dict: {"status": "string", "input_ticker": "string"}.
            // You'll need to either:
            // 1. Update price_fetcher.py to return the complex structure.
            // 2. Adapt Types::PriceHistory and this conversion logic for the simple structure.
            // The following conversion is for the complex structure.

            if (py::isinstance<py::dict>(result_obj))
            {
                py::dict py_outer_dict = result_obj.cast<py::dict>();
                for (auto item_outer : py_outer_dict)
                { // e.g., "Close", "Open"
                    std::string column_name = item_outer.first.cast<std::string>();
                    if (py::isinstance<py::dict>(item_outer.second))
                    {
                        py::dict py_inner_dict = item_outer.second.cast<py::dict>(); // e.g., {"timestamp1": price1, ...}
                        std::map<std::string, double> inner_map;
                        for (auto item_inner : py_inner_dict)
                        {
                            inner_map[item_inner.first.cast<std::string>()] = item_inner.second.cast<double>();
                        }
                        cpp_price_history[column_name] = inner_map;
                    }
                    else
                    {
                        LOG_WARN << "YFinanceProvider: Inner Python object for column '" << column_name << "' is not a dict for symbol " << symbol;
                    }
                }
                LOG_DEBUG << "YFinanceProvider: Converted Python dict to C++ PriceHistory for " << symbol;
            }
            else
            {
                LOG_WARN << "YFinanceProvider: Python function 'fetch_history' did not return a dict for symbol " << symbol;
            }
        }
        catch (const py::error_already_set &e)
        {
            LOG_ERROR << "YFinanceProvider: Python exception in fetchHistory for " << symbol << ": " << e.what();
            if (PyErr_Occurred())
                PyErr_Print();
            // Consider re-throwing as a C++ exception or returning empty/error-indicated history
        }
        catch (const std::exception &e)
        {
            LOG_ERROR << "YFinanceProvider: C++ exception in fetchHistory for " << symbol << ": " << e.what();
            // Consider re-throwing
        }

        PyGILState_Release(gstate); // Release GIL
        LOG_DEBUG << "YFinanceProvider::fetchHistory GIL released for " << symbol;
        return cpp_price_history;
    }

private:
    py::module_ price_fetcher_module_;
    // PyThreadState *_main_thread_state = nullptr; // If using PyEval_SaveThread/RestoreThread from main
};

#endif // YFINANCEPROVIDER_H