#include "Ticker.h"
#include <drogon/drogon.h>
#include <drogon/HttpResponse.h>

void Ticker::asyncHandleHttpRequest(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("Hello from Ticker!");
    callback(resp);
}