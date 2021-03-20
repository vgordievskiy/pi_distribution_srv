
#pragma once

#include <boost/random/random_device.hpp>
#include <nlohmann/json.hpp>

#include "RndData.hpp"

namespace rnd
{
    using json = nlohmann::json;

    class RndGenerator
    {   
        public:

        RndGenerator(const dto::RndData& data)
         : m_rndData(data)
        {}

        RndGenerator(dto::RndData&& data)
         : m_rndData(std::move(data))
        {}

        json generate();

        private:

        boost::random_device m_rndGen;
        dto::RndData m_rndData;
    };
}