/**
 * Provider class to enable usage of the yfinance Python module, via the pybind 11 library
 */
#ifndef YFINANCEPROVIDER_H
#define YFINANCEPROVIDER_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <map>
#include "../util/Types.h"

namespace py = pybind11;

class YFinanceProvider
{
private:
    py::module_ fetcher;

public:
    /**
     * Provides a wrapper around the yfinance Python module.
     *
     * Assumes the pybind interpreter is already running.
     */
    YFinanceProvider()
    {
        // import price_fetcher function
        fetcher = py::module_::import("price_fetcher");
    }

    /**
     * Fetches the historical price data for a given ticker and period
     * @param ticker The ticker symbol for the stock
     * @param period The period for which to fetch the data (1d,5d,1mo,3mo,6mo,1y,2y,5y,10y,ytd,max)
     * @return A map containing the historical price data, with dates as keys and prices as values.
     */
    Types::PriceHistory fetchHistory(const std::string &ticker, const std::string &period)
    {
        py::gil_scoped_acquire acq;
        return this->fetcher.attr("fetch_history")(ticker, period).cast<Types::PriceHistory>();
    }
};

#endif