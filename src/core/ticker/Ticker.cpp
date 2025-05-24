/**
 * The Ticker class handles anything relevant to a specific ticker (historical prices, dividends, etc)
 */
#include "Ticker.h"
#include "../../util/Types.h"

Ticker::Ticker(char *symbol, YFinanceProvider &yf_provider) : _symbol(symbol), _yf_provider(yf_provider) {};

Types::PriceHistory Ticker::fetchPriceHistory(const char *period)
{
    // fetch the price history for this ticker
    this->_price_history = this->_yf_provider.fetchHistory(this->_symbol, period);
    return this->_price_history;
}