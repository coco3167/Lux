#include "MetaIA.hpp"

// Survive
std::vector<const lux::City&> MetaIA::GetNeedingCity(int turn) const
{
    // return all city tiles that need to be given resources
    std::vector<const lux::City&> needyCity;
    
    for (const lux::City& city : m_cities)
    {
        // TODO add only if city doesnt meet fuel requirements
        needyCity.emplace_back(city);
    }
    return needyCity;
}

std::vector<const lux::Unit&> MetaIA::GetNeedingUnits(int turn) const
{
    // return all Units that need resources
    std::vector<const lux::Unit&> needyUnits;
    
    for (const lux::Unit& unit : m_units)
    {
        // TODO add only if city doesnt meet fuel requirements
        needyUnits.emplace_back(unit);
    }
    return needyUnits;
}

// Expand
bool MetaIA::CanBuildUnit() const
{
    size_t cityTilesNb = 0;

    for (const lux::City& city : m_cities)
    {
        cityTilesNb += city.citytiles.size();
    }

    return cityTilesNb < m_units.size();
}

// Other
void MetaIA::Update(int turn)
{
    
    // Survive
    for (auto city : GetNeedingCity(turn))
    {
        // TODO make city collect resources
    }

    for (auto unit : GetNeedingUnits(turn))
    {
        // TODO make unit collect resources
    }

    // Expand
    if(CanBuildUnit())
    {
        // TODO find a suitable city tile to build unit on
    }

    
}
