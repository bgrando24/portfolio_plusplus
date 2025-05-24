#include "TickerController.h"
#include "plugins/TickerServicePlugin.h"

using namespace drogon;

void TickerController::getTickerData(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &symbol)
{
    LOG_DEBUG << "GET /ticker/{" << symbol << "}";

    try
    {
        // fetch ticker service plugin, to fetch ticker object
        auto *plugin = drogon::app().getPlugin<TickerServicePlugin>();
        auto service = plugin->getService();
        auto &ticker_obj = service->getOrCreate(symbol);

        LOG_DEBUG << "Controller: Calling fetchPriceHistory for " << symbol;
        auto history = ticker_obj.fetchPriceHistory("1mo"); // calls YFinanceProvider
        LOG_DEBUG << "Controller: fetchPriceHistory returned for " << symbol;

        // build JSON response
        Json::Value json_response;
        json_response["symbol"] = symbol;
        json_response["history"] = Types::toJson(history);

        callback(drogon::HttpResponse::newHttpJsonResponse(json_response));
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Exception in getTickerData: " << e.what();
        auto errorResp = drogon::HttpResponse::newHttpResponse();
        errorResp->setStatusCode(drogon::k500InternalServerError);
        errorResp->setBody("Internal server error: " + std::string(e.what()));
        callback(errorResp);
    }
}