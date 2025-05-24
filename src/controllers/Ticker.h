#pragma once

#include <drogon/HttpSimpleController.h>

class Ticker : public drogon::HttpSimpleController<Ticker>
{
public:
  void asyncHandleHttpRequest(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) override;
  // Required method for Drogon even when using config file
  static void initPathRouting() {}
};
