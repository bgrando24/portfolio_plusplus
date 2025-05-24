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

    // fetch the Ticker object from the service
    auto &ticker = service->getOrCreate(symbol);
    auto history = ticker.fetchPriceHistory("1mo");

    // return as JSON
    Json::Value json_response;
    json_response["symbol"] = symbol;
    json_response["history"] = Types::toJson(history);
    callback(drogon::HttpResponse::newHttpJsonResponse(json_response));
}