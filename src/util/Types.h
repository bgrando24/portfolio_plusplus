#ifndef TYPES_H
#define TYPES_H
#include <map>
#include <string>
#include <json/json.h>

class Types
{
public:
    /**
     * Type alias for a map representing historical price data, returned from the YFinanceProvider
     */
    using PriceHistory = std::map<std::string, std::map<std::string, double>>;

    /**
     * Helper function to convert a PriceHistory object to a JSON representation
     */
    static Json::Value toJson(const PriceHistory &priceHistory)
    {
        Json::Value result(Json::objectValue);
        for (const auto &[column_name, series] : priceHistory)
        {
            Json::Value seriesJson(Json::arrayValue);
            for (const auto &[timestamp, value] : series)
            {
                Json::Value dataPoint;
                dataPoint["timestamp"] = timestamp;
                dataPoint["value"] = value;
                seriesJson.append(dataPoint);
            }
            result[column_name] = seriesJson;
        }
        return result;
    }
};

#endif