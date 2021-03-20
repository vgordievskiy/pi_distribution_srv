#include "RndGenerator.hpp"

#include <cmath>
#include <boost/random/uniform_real.hpp>

namespace rnd
{

    json RndGenerator::generate()
    {
        boost::uniform_real<> dist(0.0, 100.0);
        const double rndValue = dist(m_rndGen);

        size_t size = m_rndData.distribution.size();

        json ret = { {"name", m_rndData.name}, { "sample", nullptr } };

        double lBound = 0.0;

        const auto& dataDistr = m_rndData.distribution;
        for (size_t idx = 0; idx < size; ++idx)
        {
            auto rBound = lBound + dataDistr[idx].freq;
            if (std::isless(lBound, rndValue) && std::isgreaterequal(rBound, rndValue))
            {
                ret["sample"] = dataDistr[idx].sample;
                break;
            }
            else
            {
                lBound += dataDistr[idx].freq;
            }
        }

        return ret;
    }

}