#include "Ticker.h"
#include <drogon/HttpResponse.h>

using namespace drogon;

void Ticker::asyncHandleHttpRequest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("Hello from Ticker!");
    callback(resp);
}