#include <iostream>
#include <map>
#include <string>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <drogon/drogon.h>
#include "ext/YFinanceProvider.h"
#include "core/ticker/TickerService.h"
#include "http_controllers/plugins/TickerServicePlugin.h"

namespace py = pybind11;

int main(int argc, char *argv[])
{
    char *TICKER = nullptr;
    char *PERIOD = nullptr;
    bool DEBUG = false;
    bool TEST_FETCH = false;

    // check for CLI flags
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], (char *)"--ticker") == 0)
                TICKER = argv[i + 1];

            if (strcmp(argv[i], (char *)"--period") == 0)
                PERIOD = argv[i + 1];

            if (strcmp(argv[i], (char *)"--debug") == 0)
                DEBUG = true;

            if (strcmp(argv[i], (char *)"--test-fetch") == 0)
                TEST_FETCH = true;
        }
    }

    py::scoped_interpreter guard{}; // ensure Python is initialised and finalised correctly
    PyEval_InitThreads();           // IMPORTANT: init Python's thread support

    // create yf provider in main thread while we have GIL
    std::shared_ptr<YFinanceProvider> yf_provider;

    PyThreadState *_save = nullptr; // store main thread's state

    {
        py::gil_scoped_acquire acquire;

        auto site = py::module_::import("site");
        site.attr("addsitedir")("python/lib/python3.9/site-packages");

        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("insert")(0, "python/src");

        LOG_INFO << "Main: Creating YFinanceProvider instance...";
        yf_provider = std::make_shared<YFinanceProvider>();
        LOG_INFO << "Main: YFinanceProvider instance created.";

        if (TEST_FETCH)
        {
            // test fetch
            std::cout << "TEST FETCH: User supplied ticker: "
                      << (TICKER ? TICKER : "<none>")
                      << ", and period: "
                      << (PERIOD ? PERIOD : "<none>")
                      << std::endl;

            LOG_INFO << "Main: Performing initial test fetch from main thread...";
            auto history = yf_provider->fetchHistory(TICKER ? TICKER : "ABB.AX", PERIOD ? PERIOD : "1mo");
            if (!history.empty())
            {
                auto close_series = history.at("Close");
                std::cout << "Initial fetch from main thread (Date | Close):\n"
                          << "---------------------+-------\n";
                for (auto const &[timestamp, price] : close_series)
                {
                    std::cout << timestamp << " | " << price << "\n";
                }
            }
            else
            {
                LOG_WARN << "Main: Initial fetch from main thread did not return data";
            }
            LOG_INFO << "Main: Initial Python setup and test fetch complete";
        }
    } // GIL from py::gil_scoped_acquire is released here

    // ~~~~~~~~~~~~~ Drogon HTTP Setup ~~~~~~~~~~~~~
    TickerServicePlugin::setGlobalProvider(yf_provider);
    drogon::app().loadConfigFile("drogon-config.json");

    if (DEBUG)
    {
        std::cout << "Loading Drogon configuration..." << std::endl;
        drogon::app().setLogLevel(trantor::Logger::kTrace);
        std::cout << "Starting Drogon server on port 8080..." << std::endl;
    }

    // IMPORTANT: Release the GIL before starting Drogon's event loop
    // this allows Drogon's worker threads to acquire it
    LOG_INFO << "Main: Releasing GIL before starting Drogon app loop.";
    _save = PyEval_SaveThread(); // main thread releases the GIL

    drogon::app().run(); // Drogon application runs here - main thread blocks

    // IMPORTANT: Re-acquire the GIL when Drogon app loop finishes, before Python is finalised
    LOG_INFO << "Main: Drogon app loop finished. Re-acquiring GIL.";
    if (_save)
    { // Ensure _save was set
        PyEval_RestoreThread(_save);
    }
    // The py::scoped_interpreter (guard) destructor will also ensure GIL is acquired for finalisation

    LOG_INFO << "Main: Application shutting down.";
    return 0;
};