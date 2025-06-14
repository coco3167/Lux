#include "MetaIA.hpp"

// Survive
std::vector<const lux::City&> MetaIA::GetNeedingCity() const
{
    // return all city tiles that need to be given resources
    std::vector<const lux::City&> needyCity;
    
    for (const lux::City& city : m_cities)
    {
        if(city.NeedResources(m_turn))
        {
            needyCity.emplace_back(city);
        }
    }
    return needyCity;
}

std::vector<const lux::Unit&> MetaIA::GetNeedingUnits() const
{
    // return all Units that need resources
    std::vector<const lux::Unit&> needyUnits;
    
    for (const lux::Unit& unit : m_units)
    {
        if(unit.NeedResources(m_turn))
        {
            needyUnits.emplace_back(unit);
        }
    }
    return needyUnits;
}

void MetaIA::MakeUnitsCollectResourcesForCity(const lux::City& city)
{
    int unitsNeeded = city.ResourcesQuantityNeeded(m_turn) / RESOURCE_PER_UNIT;
    int loop = 0;
    while (unitsNeeded > 0)
    {
        if(loop >= static_cast<int>(m_units.size()))
        {
            break;
        }
            
        lux::Unit unit = m_units[loop];

        if(unit.IsAvailable())
        {
            unit.CollectResources(city);
            unitsNeeded--;
        }

        loop++;
    }
}

void MetaIA::MakeUnitsCollectResourcesForThemselves(lux::Unit& unit)
{
    if(unit.IsAvailable())
    {
        unit.CollectResources();
    }
}


// Expand
int MetaIA::NBUnitsToBuild() const
{
    size_t cityTilesNb = 0;

    for (const lux::City& city : m_cities)
    {
        cityTilesNb += city.citytiles.size();
    }

    return static_cast<int>(cityTilesNb - m_units.size());
}

void MetaIA::BuildUnits()
{
    int unitNbToBuild = NBUnitsToBuild();
    if(unitNbToBuild > 0)
    {
        // Map ordered by city score (maybe inverse the >)
        std::map<lux::City&, int, std::function<bool(int, int)>> citiesScore([](int a, int b) { return a > b; });

        // Finds a suitable city to build unit on
        for (lux::City city : m_cities)
        {
            citiesScore.emplace(city, city.UnitBuildScore());
        }

        for (auto cityScorePair : citiesScore)
        {
            if(unitNbToBuild <= 0)
                break;
            
            lux::City& chosenCity = cityScorePair.first;

            while (chosenCity.IsAvailable())
            {
                chosenCity.BuildUnit();
                unitNbToBuild--;
                if(unitNbToBuild <= 0)
                {
                    break;
                }
            }
        }
    }
}

void MetaIA::BuildCities()
{
    for (lux::Unit unit : m_units)
    {
        if(unit.IsAvailable())
        {
            unit.BuildCityTile(FindBestCityTileCell());
        }
    }
}

void MetaIA::Research()
{
    for (lux::City city : m_cities)
    {
        if(city.IsAvailable())
        {
            city.Research();
        }
    }
}

lux::Cell MetaIA::FindBestCityTileCell()
{
    // TODO find the best cell for building cityTile (keep it stored as a no go cell while a worker is doing it)
}

// Other
void MetaIA::Update(int turn)
{
    m_turn = turn;
    
// Survive
    for (const lux::City& city : GetNeedingCity())
    {
        MakeUnitsCollectResourcesForCity(city);
    }

    for (lux::Unit unit : GetNeedingUnits())
    {
        MakeUnitsCollectResourcesForThemselves(unit);
    }

    
// Expand
    BuildUnits();
    Research();
    BuildCities();
}
