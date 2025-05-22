#include "Ticker.h"

void Ticker::asyncHandleHttpRequest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto response = drogon::HttpResponse::newHttpResponse();
}
