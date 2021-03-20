#include "RndData.hpp"

namespace dto
{

std::vector<DataItem> dataItemsFromJson(const json& json)
{
    std::vector<DataItem> ret;
    if (json.is_array())
    {
        ret.reserve(json.size());

        for (const auto & item: json)
        {
            DataItem data{ item.value("freq", 0.0), item["sample"] };
            ret.push_back(std::move(data));
        }
    }

    return ret;
}

boost::optional<RndData> rndDataFromJson(const json& json)
{
    boost::optional<RndData> ret;

    if (json.contains("name") && json.contains("distribution"))
    {
        ret = RndData{json["name"].get<std::string>(), dto::dataItemsFromJson(json["distribution"])};
    }

    return ret;
}

}