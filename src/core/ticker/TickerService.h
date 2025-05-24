/**
 * The TickerService class provides a service for managing and fetching individual ticker-specific data
 * Requires the YFinanceProvider dependency
 */
#ifndef TICKERSERVICE_H
#define TICKERSERVICE_H
#include "../../ext/YFinanceProvider.h"
#include "Ticker.h"
#include <unordered_map>
#include <string>
#include <memory>

class TickerService
{
private:
    YFinanceProvider &_yf_provider;
    std::unordered_map<std::string, std::unique_ptr<Ticker>> _ticker_registry;
    // protect _ticker_registry for future multi-threaded handlers
    std::mutex _mutex;

public:
    /**
     * Requires a YFinanceProvider instance to be passed in for dependency injection
     */
    TickerService(YFinanceProvider &yf_provider);

    /**
     * Retrieves an existing Ticker instance or creates a new one if it does not exist
     * @param symbol The ticker symbol
     * @return A reference to the Ticker instance associated with the given symbol
     */
    Ticker &getOrCreate(const std::string &symbol);
};
#endif