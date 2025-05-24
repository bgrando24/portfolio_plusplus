#include "TickerController.h"
#include "plugins/TickerServicePlugin.h"

using namespace drogon;

void TickerController::getTickerData(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &symbol)
{
    // LOG_DEBUG << "GET /ticker/{" << symbol << "}";
    // // retrieve ticker service plugin
    // auto *ticker_service = drogon::app().getPlugin<TickerServicePlugin>();

    // // grab the shared_ptr<TickerService>
    // auto service = ticker_service->getService();

    // // fetch the Ticker object from the service
    // auto &ticker = service->getOrCreate(symbol);
    // auto history = ticker.fetchPriceHistory("1mo");

    // // return as JSON
    // Json::Value json_response;
    // json_response["symbol"] = symbol;
    // json_response["history"] = Types::toJson(history);
    // callback(drogon::HttpResponse::newHttpJsonResponse(json_response));

    LOG_DEBUG << "GET /ticker/{" << symbol << "}";

    try
    {
        auto *plugin = drogon::app().getPlugin<TickerServicePlugin>();
        auto service = plugin->getService();             // TickerService
        auto &ticker_obj = service->getOrCreate(symbol); // Ticker

        LOG_DEBUG << "Controller: Calling fetchPriceHistory for " << symbol;
        auto history = ticker_obj.fetchPriceHistory("1mo"); // Calls YFinanceProvider
        LOG_DEBUG << "Controller: fetchPriceHistory returned for " << symbol;

        Json::Value json_response;
        json_response["symbol"] = symbol;
        json_response["history"] = Types::toJson(history); // Assumes Types::toJson works with PriceHistory

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