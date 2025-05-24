#ifndef TickerPlugin_H
#define TickerPlugin_H
#include <drogon/plugins/Plugin.h>
#include "../../core/ticker/TickerService.h"

class TickerServicePlugin : public drogon::Plugin<TickerServicePlugin>
{
public:
    // called at startup before the HTTP server starts
    void initAndStart(const Json::Value &config) override
    {
        // construct provider
        _provider = std::make_shared<YFinanceProvider>();
        // 3) construct the service
        _service = std::make_shared<TickerService>(*_provider);
    }
    // Called at shutdown
    void shutdown() override
    {
        _service.reset();
        _provider.reset();
    }

    // make service available for controllers
    std::shared_ptr<TickerService> getService() const
    {
        return _service;
    }

private:
    std::shared_ptr<YFinanceProvider> _provider;
    std::shared_ptr<TickerService> _service;
};

#endif