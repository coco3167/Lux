#include "MetaIA.hpp"

// Survive
std::vector<CityAI&> MetaIA::GetNeedingCity()
{
    // return all city tiles that need to be given resources
    std::vector<CityAI&> needyCity;
    
    for (CityAI& city : m_cityAIs)
    {
        if(city.NeedResources(m_turn))
        {
            needyCity.emplace_back(city);
        }
    }
    return needyCity;
}

std::vector<UnitAI&> MetaIA::GetNeedingUnits()
{
    // return all Units that need resources
    std::vector<UnitAI&> needyUnits;
    
    for (UnitAI& unitAI : m_unitAIs)
    {
        if(unitAI.NeedResources(m_turn))
        {
            needyUnits.emplace_back(unitAI);
        }
    }
    return needyUnits;
}

void MetaIA::MakeUnitsCollectResourcesForCity(CityAI& city)
{
    int unitsNeeded = city.ResourcesQuantityNeeded(m_turn) / RESOURCE_PER_UNIT;
    int loop = 0;
    while (unitsNeeded > 0)
    {
        if(loop >= static_cast<int>(m_unitAIs.size()))
        {
            break;
        }
            
        UnitAI& unitAI = m_unitAIs[loop];

        if(unitAI.IsAvailable())
        {
            unitAI.CollectResources(city);
            unitsNeeded--;
        }

        loop++;
    }
}

void MetaIA::MakeUnitsCollectResourcesForThemselves(UnitAI& unit)
{
    if(unit.IsAvailable())
    {
        unit.CollectResources();
    }
}


// Expand
int MetaIA::NBUnitsToBuild() const
{
    /*size_t cityTilesNb = 0;

    for (const CityTileAI& cityAI : m_cityTileAIs)
    {
        cityTilesNb += cityAI.City.citytiles.size();
    }*/

    return static_cast<int>(m_cityTileAIs.size() - m_unitAIs.size());
}

void MetaIA::BuildUnits(const lux::GameMap& gameMap)
{    
    int unitNbToBuild = NBUnitsToBuild();
    if(unitNbToBuild > 0)
    {
        // Map ordered by city score (maybe inverse the >)
        std::map<CityTileAI&, int, std::function<bool(int, int)>> citiesScore([](int a, int b) { return a > b; });

        // Finds a suitable city to build unit on
        for (CityTileAI& cityAI : m_cityTileAIs)
        {
            citiesScore.emplace(cityAI, cityAI.UnitBuildScore(gameMap));
        }

        for (std::pair<CityTileAI&, int> cityScorePair : citiesScore)
        {
            if(unitNbToBuild <= 0)
                break;
            
            CityTileAI& chosenCity = cityScorePair.first;

            if (chosenCity.IsAvailable())
            {
                m_stringResult.append(chosenCity.BuildUnit(m_unitAIs));
                unitNbToBuild--;
            }
        }
    }
}

void MetaIA::BuildCities()
{
    for (UnitAI& unitAI : m_unitAIs)
    {
        if(unitAI.IsAvailable() && unitAI.Unit.isWorker())
        {
            unitAI.BuildCityTile();
        }
    }
}

void MetaIA::Research()
{
    for (CityTileAI& cityAI : m_cityTileAIs)
    {
        if(cityAI.IsAvailable())
        {
            m_stringResult.append(cityAI.Research());
        }
    }
}


// Other
std::string MetaIA::Update(int turn, const lux::GameMap& gameMap)
{
    m_stringResult.clear();
    m_turn = turn;
    
// Survive
    for (CityAI& city : GetNeedingCity())
    {
        MakeUnitsCollectResourcesForCity(city);
    }

    for (UnitAI& unit : GetNeedingUnits())
    {
        MakeUnitsCollectResourcesForThemselves(unit);
    }

    
// Expand
    BuildUnits(gameMap);
    Research();
    BuildCities();

    return m_stringResult;
}
