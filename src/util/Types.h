#ifndef TYPES_H
#define TYPES_H
#include <map>
#include <string>

class Types
{
public:
    /**
     * Type alias for a map representing historical price data, returned from the YFinanceProvider
     */
    using PriceHistory = std::map<std::string, std::map<std::string, double>>;
};

#endif