#ifndef TickerPlugin_H
#define TickerPlugin_H
#include <drogon/plugins/Plugin.h>
#include "../../core/ticker/TickerService.h"
#include "../../ext/YFinanceProvider.h"

class TickerServicePlugin : public drogon::Plugin<TickerServicePlugin>
{
public:
    // Static method to set global provider
    static void setGlobalProvider(std::shared_ptr<YFinanceProvider> provider)
    {
        _global_provider = provider;
    }

    // called at startup before the HTTP server starts
    void initAndStart(const Json::Value &config) override
    {
        if (!_global_provider)
        {
            LOG_ERROR << "YFinanceProvider not set! Call setGlobalProvider() first.";
            return;
        }

        // construct the service with global provider
        _service = std::make_shared<TickerService>(*_global_provider);
        LOG_INFO << "TickerServicePlugin initialized with global provider";
    }

    // Called at shutdown
    void shutdown() override
    {
        _service.reset();
        LOG_INFO << "TickerServicePlugin shutdown";
    }

    // make service available for controllers
    std::shared_ptr<TickerService> getService() const
    {
        return _service;
    }

private:
    static std::shared_ptr<YFinanceProvider> _global_provider;
    std::shared_ptr<TickerService> _service;
};

#endif