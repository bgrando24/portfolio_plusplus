/**
 * The Ticker class handles anything relevant to a specific ticker (historical prices, dividends, etc)
 */
#include <drogon/drogon.h>
#include "Ticker.h"
#include "../../util/Types.h"

Ticker::Ticker(const std::string symbol, YFinanceProvider &yf_provider) : _symbol(symbol), _yf_provider(yf_provider) {};

Types::PriceHistory Ticker::fetchPriceHistory(const std::string period)
{
    LOG_DEBUG << "Fetching price history for ticker: " << this->_symbol << " with period: " << period;
    // fetch the price history for this ticker
    this->_price_history = this->_yf_provider.fetchHistory(this->_symbol, period);
    LOG_DEBUG << "Fetched price history for ticker: " << this->_symbol << ", period: " << period;
    return this->_price_history;
}