#pragma once

#include <string>
#include <vector>
#include <iterator>

#include <boost/optional.hpp>

#include <nlohmann/json.hpp>

namespace dto
{
using json = nlohmann::json;

struct DataItem
{
    double freq;
    json sample;
};

struct RndData
{
    std::string name;
    std::vector<DataItem> distribution;
};


std::vector<DataItem> dataItemsFromJson(const json& json);

boost::optional<RndData> rndDataFromJson(const json& json);


} // end namespace data