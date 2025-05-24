#include <iostream>
#include <map>
#include <string>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <drogon/drogon.h>
#include "ext/YFinanceProvider.h"
#include "core/ticker/TickerService.h"

namespace py = pybind11;

int main(int argc, char *argv[])
{
    char *ticker = nullptr;
    char *period = nullptr;
    bool DEBUG = false;

    // check for CLI flags
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], (char *)"--ticker") == 0)
                ticker = argv[i + 1];

            if (strcmp(argv[i], (char *)"--period") == 0)
                period = argv[i + 1];

            if (strcmp(argv[i], (char *)"--debug") == 0)
                DEBUG = true;
        }
    }

    std::cout << "User supplied ticker: " << (ticker ? ticker : "<none>") << ", and period: " << (period ? period : "<none>") << std::endl;

    // start python interpreter
    py::scoped_interpreter guard{};

    // enable venv packages
    auto site = py::module_::import("site");
    site.attr("addsitedir")("python/lib/python3.9/site-packages");

    // tell interpreter where to look
    py::module_ sys = py::module_::import("sys");
    sys.attr("path").attr("insert")(0, "python/src");

    // init YFinanceProvider and fetch ticker history
    YFinanceProvider yf_provider;
    auto history = yf_provider.fetchHistory(ticker ? ticker : "ABB.AX", period ? period : "1mo");
    // grab the Close‐price series (throws if there is no "Close" column)
    auto close_series = history.at("Close");

    // print the ticker price data
    std::cout << "Date                 | Close\n"
              << "---------------------+-------\n";

    for (auto const &[timestamp, price] : close_series)
    {
        std::cout << timestamp << " | " << price << "\n";
    }

    // ~~~~~~~~~~~~~ Drogon HTTP Setup ~~~~~~~~~~~~~
    // init ticker service
    auto ticker_service = std::make_shared<TickerService>(yf_provider);

    // drogon config file
    drogon::app().loadConfigFile("drogon-config.json");
    // store ticker service in custom data

    if (DEBUG)
    {
        std::cout << "Loading Drogon configuration..." << std::endl;
        drogon::app().setLogLevel(trantor::Logger::kTrace);
        std::cout << "Starting Drogon server on port 8080..." << std::endl;
        std::cout << "Available routes should include /ticker" << std::endl;
    }

    drogon::app().run();
    return 0;
};