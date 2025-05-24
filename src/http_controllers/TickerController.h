#pragma once

#include <drogon/HttpController.h>

class TickerController : public drogon::HttpController<TickerController>
{
public:
  /**
   * Requests to this endpoint trigger fetching ticker data for a given ticker symbol
   */
  void getTickerData(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback, const std::string &symbol);

  METHOD_LIST_BEGIN
  // register the HTTP GET method for fetching ticker data
  ADD_METHOD_TO(TickerController::getTickerData, "/ticker/{1}", drogon::Get);
  METHOD_LIST_END
};
