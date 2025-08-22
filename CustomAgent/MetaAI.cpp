#include "MetaAI.hpp"

#include <algorithm>

MetaAI::MetaAI(GameDatas& gameDatas):
    m_gameDatas(gameDatas)
{

}

// Other
void MetaAI::Update(const int turn)
{
    m_turn = turn;

    ManageAILives();
    GiveOrders();
    UpdateSubAIs();
    DrawDebug();
}

void MetaAI::ManageAILives()
{
    Player& player = m_gameDatas.Owner;

    std::vector<City*> cities = {};
    cities.reserve(player.cities.size());

    std::vector<CityTile*> cityTiles = {};
    cityTiles.reserve(cities.size() * 5);

    for (const std::pair<string, City>& pair : player.cities) 
    {
        City* city = &player.cities.at(pair.first);
        cities.push_back(city);

        for (CityTile& tile : city->citytiles)
        {
            cityTiles.push_back(&tile);
        }
    }

    std::vector<Unit*> workers = {};
    workers.reserve(player.units.size());

    std::vector<Unit*> carts = {};
    carts.reserve(player.units.size());

    for (Unit& unit : player.units) 
    {
        if (unit.isWorker()) 
        {
            workers.push_back(&unit);
        }
        else 
        {
            carts.push_back(&unit);
        }
    }

    ManageSubAILife(m_workerAIs, workers);
    ManageSubAILife(m_cartAIs, carts);
    ManageSubAILife(m_cityAIs, cities);
    ManageSubAILife(m_cityTileAIs, cityTiles);
}

void MetaAI::GiveOrders()
{
    // Survive

    for (CityAI* city : GetNeedingCity())
    {
        MakeUnitsCollectResourcesForCity(*city);
    }

    for (WorkerAI* unit : GetNeedingUnits())
    {
        MakeUnitsCollectResourcesForThemselves(*unit);
    }


    // Expand
    BuildUnits();
    Research();
    BuildCities();
}

void MetaAI::UpdateSubAIs()
{
    for (WorkerAI& worker : m_workerAIs) 
    {
        worker.Update();
    }
}

void MetaAI::DrawDebug()
{
    m_gameDatas.AddAction(std::move(Annotate::sidetext(Utils::FormatString("Turn : %i", m_turn))));
    m_gameDatas.AddAction(std::move(Annotate::sidetext(" ")));
    m_gameDatas.AddAction(std::move(Annotate::sidetext("Sub AIs Count : ")));
    m_gameDatas.AddAction(std::move(Annotate::sidetext(Utils::FormatString("    - Workers : %i - %i", m_workerAIs.size(), m_gameDatas.Owner.units.size()))));
    m_gameDatas.AddAction(std::move(Annotate::sidetext(Utils::FormatString("    - Carts : %i", m_cartAIs.size()))));
    m_gameDatas.AddAction(std::move(Annotate::sidetext(Utils::FormatString("    - Cities : %i - %i", m_cityAIs.size(), m_gameDatas.Owner.cities.size()))));
    m_gameDatas.AddAction(std::move(Annotate::sidetext(Utils::FormatString("    - CityTiles : %i - %i", m_cityTileAIs.size(), m_gameDatas.Owner.cityTileCount))));

    for (WorkerAI& worker : m_workerAIs)
    {
        worker.DrawDebug(m_gameDatas);
    }

    for (CartAI& cart : m_cartAIs)
    {
        cart.DrawDebug(m_gameDatas);
    }

    for (CityAI& city : m_cityAIs)
    {
        city.DrawDebug(m_gameDatas);
    }

    for (CityTileAI& cityTile : m_cityTileAIs)
    {
        cityTile.DrawDebug(m_gameDatas);
    }
}

// Survive
std::vector<CityAI*> MetaAI::GetNeedingCity()
{
    // return all city tiles that need to be given resources
    std::vector<CityAI*> needyCity;
    needyCity.reserve(m_cityAIs.size());
    
    for (CityAI& city : m_cityAIs)
    {
        if(city.NeedResources(m_turn))
        {
            needyCity.emplace_back(&city);
        }
    }
    return needyCity;
}

std::vector<WorkerAI*> MetaAI::GetNeedingUnits()
{
    // return all Units that need resources
    std::vector<WorkerAI*> needyUnits;
    needyUnits.reserve(m_workerAIs.size());

    for (WorkerAI& worker : m_workerAIs)
    {
        if (worker.NeedResources(m_turn))
        {
            needyUnits.emplace_back(&worker);
        }
    }
    /*
    for (CartAI& cart : m_cartAIs)
    {
        if (cart.NeedResources(m_turn))
        {
            needyUnits.emplace_back(&cart);
        }
    }
    */
    return needyUnits;
}

void MetaAI::MakeUnitsCollectResourcesForCity(CityAI& city)
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

void MetaAI::MakeUnitsCollectResourcesForThemselves(WorkerAI& unit)
{
    if(unit.IsAvailable())
    {
        unit.CollectResources();
    }
}


// Expand
int MetaAI::NBUnitsToBuild() const
{
    return static_cast<int>(m_cityTileAIs.size() - m_gameDatas.Owner.units.size());
}

void MetaAI::BuildUnits()
{
    int unitNbToBuild = NBUnitsToBuild();
    if(unitNbToBuild > 0)
    {
        std::vector<CityTileAI*> sortedCityTiles{};
        sortedCityTiles.reserve(m_cityTileAIs.size());

        // Finds a suitable city to build unit on
        for (CityTileAI& cityAI : m_cityTileAIs)
        {
            sortedCityTiles.push_back(&cityAI);
        }

        GameDatas& datas = m_gameDatas;

        std::sort(sortedCityTiles.begin(), sortedCityTiles.end(),
            [&datas](CityTileAI* a, CityTileAI* b)
            {
                return a->UnitBuildScore(datas.Map) < b->UnitBuildScore(datas.Map);
            });

        for (CityTileAI* chosenCityTile : sortedCityTiles)
        {
            if(unitNbToBuild <= 0)
            {
                break;
            }

            while (chosenCityTile->IsAvailable())
            {
                m_gameDatas.AddAction(std::move(chosenCityTile->BuildUnit(m_workerAIs.size(), m_cartAIs.size())));
                unitNbToBuild--;
                if(unitNbToBuild <= 0)
                {
                    break;
                }
            }
        }
    }
}

void MetaAI::BuildCities()
{
    for (WorkerAI& workerAI : m_workerAIs)
    {
        if(workerAI.IsAvailable() && workerAI.ManagedObject->isWorker())
        {
            workerAI.BuildCityTile();
        }
    }
}

void MetaAI::Research()
{
    for (CityTileAI& cityTile : m_cityTileAIs)
    {
        if(cityTile.IsAvailable())
        {
            m_gameDatas.AddAction(std::move(cityTile.Research())); // Insert command
        }
    }
}

