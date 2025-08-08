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
    size_t cityTilesNb = 0;

    for (const CityAI& cityAI : m_cityAIs)
    {
        cityTilesNb += cityAI.City.citytiles.size();
    }

    return static_cast<int>(cityTilesNb - m_unitAIs.size());
}

void MetaIA::BuildUnits()
{
    int unitNbToBuild = NBUnitsToBuild();
    if(unitNbToBuild > 0)
    {
        // Map ordered by city score (maybe inverse the >)
        std::map<CityAI&, int, std::function<bool(int, int)>> citiesScore([](int a, int b) { return a > b; });

        // Finds a suitable city to build unit on
        for (CityAI& cityAI : m_cityAIs)
        {
            citiesScore.emplace(cityAI, cityAI.UnitBuildScore());
        }

        for (std::pair<CityAI&, int> cityScorePair : citiesScore)
        {
            if(unitNbToBuild <= 0)
                break;
            
            CityAI& chosenCity = cityScorePair.first;

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
    for (CityAI& cityAI : m_cityAIs)
    {
        if(cityAI.IsAvailable())
        {
            cityAI.Research();
        }
    }
}


// Other
void MetaIA::Update(int turn)
{
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
    BuildUnits();
    Research();
    BuildCities();
}
