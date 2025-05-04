#include <iostream>
#include <map>
#include <string>
#include <pybind11/embed.h>

// pybind namespace
namespace py = pybind11;

int main()
{
    std::cout << "Hello, world!" << std::endl;

    // start python interpreter
    py::scoped_interpreter guard{};

    // tell interpreter where to look
    py::module_ sys = py::module_::import("sys");
    sys.attr("path").attr("insert")(0, "python/src");

    // import yfinance python file
    py::module_ fetcher = py::module_::import("price_fetcher");

    // call fetch_history function from yfinance python file
    // returns nexted stdd:map<string, map:std::string, double>>
    auto history = fetcher.attr("fetch_history")("ABB.AX", "1mo").cast<std::map<std::string, std::map<std::string, double>>>();

    // print data
    for (auto const &[date, row] : history)
    {
        double close = row.at("Close");
        std::cout << date << " | " << close << "\n";
    }

    return 0;
};