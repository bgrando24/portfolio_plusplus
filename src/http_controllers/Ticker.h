#pragma once

#include <drogon/HttpSimpleController.h>

class Ticker : public drogon::HttpSimpleController<Ticker>
{
public:
  void asyncHandleHttpRequest(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) override;
  PATH_LIST_BEGIN
    PATH_ADD("/ticker", drogon::Get);
  PATH_LIST_END
};
