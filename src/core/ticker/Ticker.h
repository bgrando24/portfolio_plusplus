#ifndef TICKER_H
#define TICKER_H

#include "../../util/Types.h"
#include "../../ext/YFinanceProvider.h"

class Ticker
{
private:
    // ticker symbol
    char *_symbol;
    // yfinance provider dependecy injection
    YFinanceProvider &_yf_provider;
    // historical price data state
    Types::PriceHistory _price_history;

public:
    /**
     * @param symbol The ticker symbol for the stock (e.g., "AAPL" for Apple Inc.)
     */
    Ticker(char *symbol, YFinanceProvider &yf_provider);

    /**
     * Fetches the historical price data for this ticker
     * @param period The period for which to fetch the data (1d,5d,1mo,3mo,6mo,1y,2y,5y,10y,ytd,max)
     * @return A map containing the historical price data, with dates as keys and prices as values.
     */
    Types::PriceHistory fetchPriceHistory(const char *period = "1mo");
};

#endif