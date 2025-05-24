#ifndef TICKER_H
#define TICKER_H

#include "../../util/Types.h"

class Ticker
{
private:
    char *_symbol;

public:
    /**
     * @param symbol The ticker symbol for the stock (e.g., "AAPL" for Apple Inc.)
     */
    Ticker(char *symbol);

    /**
     * Fetches the historical price data for this ticker
     * @param period The period for which to fetch the data (1d,5d,1mo,3mo,6mo,1y,2y,5y,10y,ytd,max)
     * @return A map containing the historical price data, with dates as keys and prices as values.
     */
    Types::PriceHistory fetchPriceHistory(const char *period = "1mo");
};

#endif