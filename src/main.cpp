#include <iostream>
#include <map>
#include <string>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <drogon/drogon.h>

namespace py = pybind11;

int main(int argc, char *argv[])
{
    char *ticker = nullptr;
    char *period = nullptr;

    // check if --ticker and --period CLI arg are used
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], (char *)"--ticker") == 0)
                ticker = argv[i + 1];

            if (strcmp(argv[i], (char *)"--period") == 0)
                period = argv[i + 1];
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

    // import yfinance python file
    py::module_ fetcher = py::module_::import("price_fetcher");

    // call fetch_history function from yfinance python file
    auto history = fetcher.attr("fetch_history")(ticker ? ticker : "ABB.AX", period ? period : "1mo").cast<std::map<std::string, std::map<std::string, double>>>();

    // (debug) print data
    // grab the Close‐price series (throws if there is no "Close" column)
    auto close_series = history.at("Close");

    std::cout << "Date                 | Close\n"
              << "---------------------+-------\n";

    for (auto const &[timestamp, price] : close_series)
    {
        std::cout << timestamp << " | " << price << "\n";
    }

    // drogon setup with debug info
    std::cout << "Loading Drogon configuration..." << std::endl;
    drogon::app().loadConfigFile("drogon-config.json");
    
    // Add some debug logging
    drogon::app().setLogLevel(trantor::Logger::kTrace);
    
    std::cout << "Starting Drogon server on port 8080..." << std::endl;
    std::cout << "Available routes should include /ticker" << std::endl;
    
    drogon::app().run();
    return 0;
};