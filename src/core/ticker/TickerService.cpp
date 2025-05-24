#include "TickerService.h"
#include "Ticker.h"
#include <memory>
#include <drogon/drogon.h>

TickerService::TickerService(YFinanceProvider &yf_provider) : _yf_provider(yf_provider) {};

Ticker &TickerService::getOrCreate(const std::string &symbol)
{
    LOG_DEBUG << "Calling TickerService getOrCreate for symbol: " << symbol;
    ;
    // .find() returns an iterator to the element if found, or end() if not found
    auto ticker_iterator = _ticker_registry.find(symbol);
    // ticker not found, create new instance and add to resigstry map
    if (ticker_iterator == _ticker_registry.end())
    {
        // Create and insert in one step, get iterator to inserted element
        auto [inserted_iterator, success] = _ticker_registry.emplace(
            symbol,
            std::make_unique<Ticker>(symbol, _yf_provider));
        return *inserted_iterator->second;
    }
    // ticker already exists, return it
    return *ticker_iterator->second;
}