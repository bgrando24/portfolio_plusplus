#include <iostream>
#include <map>
#include <string>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;

int main()
{
    std::cout << "Hello, world!" << std::endl;

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
    auto history = fetcher.attr("fetch_history")("ABB.AX", "1mo").cast<std::map<std::string, std::map<std::string, double>>>();

    // (debug) print data
    // grab the Close‐price series (throws if there is no "Close" column)
    auto close_series = history.at("Close");

    std::cout << "Date                 | Close\n"
              << "---------------------+-------\n";

    for (auto const &[timestamp, price] : close_series)
    {
        std::cout << timestamp << " | " << price << "\n";
    }

    return 0;
};