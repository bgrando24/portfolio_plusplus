#ifndef YFINANCEPROVIDER_H
#define YFINANCEPROVIDER_H

#include <string>
#include <map>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <drogon/drogon.h>
#include "../util/Types.h"

namespace py = pybind11;

/**
 * The YFinanceProvider class provides an interface to the python fetch_price module
 */
class YFinanceProvider
{
public:
    YFinanceProvider()
    {
        // attempt importing the module
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
        }
    }

    /**
     * Fetches historical price data for a given symbol and period
     * @param symbol The stock ticker symbol
     * @param period The period for which to fetch the data (1d, 5d, 1mo, 3mo, 6mo, 1y, 2y, 5y, 10y, ytd, max)
     * @return A PriceHistory object containing the historical price data
     */
    Types::PriceHistory fetchHistory(const std::string &symbol, const std::string &period)
    {
        LOG_DEBUG << "YFinanceProvider::fetchHistory called for symbol: " << symbol << ", period: " << period;

        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure(); // acquire GIL for this Drogon worker thread

        Types::PriceHistory cpp_price_history;

        try
        {
            LOG_DEBUG << "YFinanceProvider: Calling Python 'fetch_history' for " << symbol;
            py::object result_obj = price_fetcher_module_.attr("fetch_history")(symbol, period);
            LOG_DEBUG << "YFinanceProvider: Python 'fetch_history' call returned for " << symbol;

            // IMPORTANT: The conversion logic below assumes price_fetcher.py returns a dict compatible with Types::PriceHistory
            if (py::isinstance<py::dict>(result_obj))
            {
                py::dict py_outer_dict = result_obj.cast<py::dict>();
                for (auto item_outer : py_outer_dict)
                {
                    // e.g., "Close", "Open"
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
        }
        catch (const std::exception &e)
        {
            LOG_ERROR << "YFinanceProvider: C++ exception in fetchHistory for " << symbol << ": " << e.what();
        }

        // release GIL
        PyGILState_Release(gstate);
        LOG_DEBUG << "YFinanceProvider::fetchHistory GIL released for " << symbol;
        return cpp_price_history;
    }

private:
    // PyThreadState *_main_thread_state = nullptr; // If using PyEval_SaveThread/RestoreThread from main
    py::module_ price_fetcher_module_;
};

#endif // YFINANCEPROVIDER_H