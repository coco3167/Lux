#include "MetaIA.hpp"



MetaIA::MetaIA(GameDatas& gameDatas):
    m_gameDatas(gameDatas)
{

}

// Other
void MetaIA::Update(const int turn)
{
    m_turn = turn;

    ManageAILives();
    GiveOrders();
    UpdateSubAIs();
}

void MetaIA::ManageAILives()
{
    Player& player = m_gameDatas.Owner;

    std::vector<const City&> cities = {};
    cities.reserve(player.cities.size());

    for (const std::pair<string, City>& pair : player.cities) 
    {
        cities.push_back(player.cities.at(pair.first));
    }

    std::vector<const Unit&> workers = {};
    workers.reserve(player.units.size());

    std::vector<const Unit&> carts = {};
    carts.reserve(player.units.size());

    for (const Unit& unit : player.units) 
    {
        if (unit.isWorker()) 
        {
            workers.push_back(unit);
        }
        else 
        {
            carts.push_back(unit);
        }
    }

    ManageSubAILife(m_workerAIs, workers);
    ManageSubAILife(m_cartAIs, carts);
    ManageSubAILife(m_cityAIs, cities);
}

void MetaIA::GiveOrders()
{
    // Survive

    for (CityAI& city : GetNeedingCity())
    {
        MakeUnitsCollectResourcesForCity(city);
    }

    for (WorkerAI& unit : GetNeedingUnits())
    {
        MakeUnitsCollectResourcesForThemselves(unit);
    }


    // Expand
    BuildUnits();
    Research();
    BuildCities();
}

void MetaIA::UpdateSubAIs()
{
    for (WorkerAI& worker : m_workerAIs) 
    {
        worker.Update();
    }
}

// Survive
std::vector<CityAI&> MetaIA::GetNeedingCity() const
{
    // return all city tiles that need to be given resources
    std::vector<CityAI&> needyCity;
    needyCity.reserve(m_cityAIs.size());
    
    for (const CityAI& city : m_cityAIs)
    {
        if(city.NeedResources(m_turn))
        {
            needyCity.emplace_back(city);
        }
    }
    return needyCity;
}

std::vector<WorkerAI&> MetaIA::GetNeedingUnits() const
{
    // return all Units that need resources
    std::vector<WorkerAI&> needyUnits;
    needyUnits.reserve(m_workerAIs.size());
    
    for (const WorkerAI& worker : m_workerAIs)
    {
        if(worker.NeedResources(m_turn))
        {
            needyUnits.emplace_back(worker);
        }
    }
    return needyUnits;
}

void MetaIA::MakeUnitsCollectResourcesForCity(CityAI& city)
{
    int unitsNeeded = city.ResourcesQuantityNeeded(m_turn) / RESOURCE_PER_UNIT;
    int loop = 0;

    int workersCount = static_cast<int>(m_workerAIs.size());

    while (unitsNeeded > 0)
    {
        if(loop >= workersCount)
        {
            break;
        }
            
        WorkerAI& unitAI = m_workerAIs[loop];

        if(unitAI.IsAvailable())
        {
            unitAI.CollectResources(city);
            unitsNeeded--;
        }

        loop++;
    }
}

void MetaIA::MakeUnitsCollectResourcesForThemselves(WorkerAI& unit)
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
        cityTilesNb += cityAI.ManagedObject.citytiles.size();
    }

    return static_cast<int>(cityTilesNb - m_workerAIs.size());
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
    for (WorkerAI& workerAI : m_workerAIs)
    {
        if(workerAI.IsAvailable() && workerAI.ManagedObject.isWorker())
        {
            workerAI.BuildCityTile();
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
