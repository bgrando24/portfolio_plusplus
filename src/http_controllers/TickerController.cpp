#include "TickerController.h"
#include "plugins/TickerServicePlugin.h"

using namespace drogon;

void TickerController::getTickerData(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &symbol)
{
    LOG_DEBUG << "GET /ticker/{" << symbol << "}";
    // retrieve ticker service plugin
    auto *ticker_service = drogon::app().getPlugin<TickerServicePlugin>();

    // grab the shared_ptr<TickerService>
    auto service = ticker_service->getService();
}