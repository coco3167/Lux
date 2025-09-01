#include "MetaAI.hpp"

#include <algorithm>

#include "Debug.h"


MetaAI::MetaAI(GameDatas* gameDatas):
    m_gameDatas(gameDatas)
{
    m_gameDatas->WorkerAIs = &m_workerAIs;
    m_gameDatas->CartAIs = &m_cartAIs;
}

// Other
void MetaAI::Update(const int turn)
{
    m_turn = turn;

    Debug::Log("ManageLives");
    ManageAILives();

    Debug::Log("GiveOrders");
    GiveOrders();

    Debug::Log("Update SubAis");
    UpdateSubAIs();

    Debug::Log("Debug");
    DrawDebug();
}

void MetaAI::ManageAILives()
{
    ResetSubAIManagedObject(m_workerAIs);
    ResetSubAIManagedObject(m_cartAIs);
    ResetSubAIManagedObject(m_cityAIs);

    Player& player = *(m_gameDatas->Owner);

    std::vector<City*> cities = {};
    cities.reserve(player.cities.size());

    std::vector<CityTile*> cityTiles = {};
    cityTiles.reserve(cities.size() * 5);

    for (std::map<string, City>::iterator it = player.cities.begin(); it != player.cities.end(); it++)
    {
        City* city = &it->second;
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
    CreateCityTilesAIs(cityTiles);
}

void MetaAI::CreateCityTilesAIs(std::vector<CityTile*>& allTiles)
{
    m_cityTileAIs.clear();
    m_cityTileAIs.reserve(allTiles.size());

    for (CityTile* tile : allTiles)
    {
        m_cityTileAIs.emplace_back(tile);
    }
}

void MetaAI::GiveOrders()
{
    // Survive
    std::vector<CityAI*> needingCities = std::move(GetNeedingCity());
    for (CityAI* city : needingCities)
    {
        MakeUnitsCollectResourcesForCity(*city);
    }

    for (WorkerAI* unit : GetNeedingWorkers())
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
    Debug::Log("Update Workers");
    for (WorkerAI& worker : m_workerAIs)
    {
        worker.Update();
    }

    Debug::Log("Update Carts");
    for (CartAI& cart : m_cartAIs)
    {
        cart.Update();
    }
}

void MetaAI::DrawDebug()
{
    m_gameDatas->AddAction(std::move(Annotate::sidetext(Utils::FormatString("Turn : %i", m_turn))));
    m_gameDatas->AddAction(std::move(Annotate::sidetext("Sub AIs Count : ")));
    m_gameDatas->AddAction(std::move(Annotate::sidetext(Utils::FormatString("    - Workers : %i - %i", m_workerAIs.size(), m_gameDatas->Owner->units.size()))));
    m_gameDatas->AddAction(std::move(Annotate::sidetext(Utils::FormatString("    - Carts : %i", m_cartAIs.size()))));
    m_gameDatas->AddAction(std::move(Annotate::sidetext(Utils::FormatString("    - Cities : %i - %i", m_cityAIs.size(), m_gameDatas->Owner->cities.size()))));
    m_gameDatas->AddAction(std::move(Annotate::sidetext(Utils::FormatString("    - CityTiles : %i - %i", m_cityTileAIs.size(), m_gameDatas->Owner->cityTileCount))));

    for (WorkerAI& worker : m_workerAIs)
    {
        worker.DrawDebug(*m_gameDatas);
    }

    for (CartAI& cart : m_cartAIs)
    {
        cart.DrawDebug(*m_gameDatas);
    }

    for (CityAI& city : m_cityAIs)
    {
        city.DrawDebug(*m_gameDatas);
    }
}

// Survive
std::vector<CityAI*> MetaAI::GetNeedingCity()
{
    // return all cities that need to be given resources
    std::vector<CityAI*> needyCity;
    needyCity.reserve(m_cityAIs.size());
    
    for (CityAI& city : m_cityAIs)
    {
        if(city.NeedResources(m_turn))
        {
            needyCity.push_back(&city);
        }
    }
    return needyCity;
}

std::vector<WorkerAI*> MetaAI::GetNeedingWorkers()
{
    // return all Units that need resources
    std::vector<WorkerAI*> needyWorkers;
    needyWorkers.reserve(m_workerAIs.size());

    for (WorkerAI& worker : m_workerAIs)
    {
        if (worker.NeedResources(m_turn))
        {
            needyWorkers.push_back(&worker);
        }
    }
    return needyWorkers;
}

std::vector<CartAI*> MetaAI::GetNeedingCarts()
{
    std::vector<CartAI*> needyCarts;
    needyCarts.reserve(m_cartAIs.size());

    for (CartAI& cart : m_cartAIs)
    {
        if (cart.NeedResources(m_turn))
        {
            needyCarts.push_back(&cart);
        }
    }

    return needyCarts;
}

void MetaAI::MakeUnitsCollectResourcesForCity(CityAI& city)
{
    int unitsNeeded = city.ResourcesQuantityNeeded(m_turn) / RESOURCE_PER_UNIT;
    int iWorker = 0;
    int iCart = 0;


    // Arbitrary 
    Position cityPosition = city.ManagedObject->citytiles[0].pos;

    std::vector<WorkerAI*> closestAvailableWorkers = GetAvailableWorkersSortedByDistance(cityPosition);
    std::vector<CartAI*> closestAvailableCarts = GetAvailableCartsSortedByDistance(cityPosition);

    int workersCount = static_cast<int>(closestAvailableWorkers.size());
    int cartsCount = static_cast<int>(closestAvailableCarts.size());

    while (unitsNeeded > 0)
    {
        if(iWorker >= workersCount)
        {
            break;
        }
            
        closestAvailableWorkers[iWorker++]->CollectResources(city);
        unitsNeeded--;

        // Adds a cart to help the worker
        if (iCart >= cartsCount)
        {
            continue;
        }

        closestAvailableCarts[iCart++]->TryGoResupply(&city);
    }
}

void MetaAI::MakeUnitsCollectResourcesForThemselves(WorkerAI& worker)
{
    if(worker.IsAvailable())
    {
        worker.CollectResources();
    }
}


// Expand
int MetaAI::NBUnitsToBuild() const
{
    return static_cast<int>(m_cityTileAIs.size() - m_gameDatas->Owner->units.size());
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

        GameDatas* datas = m_gameDatas;

        std::sort(sortedCityTiles.begin(), sortedCityTiles.end(),
            [datas](CityTileAI* a, CityTileAI* b)
            {
                return a->UnitBuildScore(datas->Map) < b->UnitBuildScore(datas->Map);
            });

        for (CityTileAI* chosenCityTile : sortedCityTiles)
        {
            if(unitNbToBuild <= 0)
            {
                break;
            }

            while (chosenCityTile->IsAvailable())
            {
                m_gameDatas->AddAction(std::move(chosenCityTile->BuildUnit(m_workerAIs.size(), m_cartAIs.size())));
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
        if(workerAI.IsAvailable())
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
            m_gameDatas->AddAction(std::move(cityTile.Research()));
        }
    }
}

std::vector<WorkerAI*> MetaAI::GetAvailableWorkersSortedByDistance(Position startPosition)
{
    std::vector<WorkerAI*> result = {};
    result.reserve(m_workerAIs.size());

    for (WorkerAI& worker : m_workerAIs)
    {
        if (!worker.IsAvailable())
        {
            continue;
        }
        result.push_back(&worker);
    }

    std::sort(result.begin(), result.end(),
        [startPosition](WorkerAI* a, WorkerAI* b)
        {
            return a->ManagedObject->pos.distanceTo(startPosition) < b->ManagedObject->pos.distanceTo(startPosition);
        });
    return result;
}

std::vector<CartAI*> MetaAI::GetAvailableCartsSortedByDistance(Position startPosition)
{
    std::vector<CartAI*> result = {};
    result.reserve(m_cartAIs.size());

    for (CartAI& cart : m_cartAIs)
    {
        if (!cart.IsAvailable())
        {
            continue;
        }
        result.push_back(&cart);
    }

    std::sort(result.begin(), result.end(),
        [startPosition](CartAI* a, CartAI* b)
        {
            return a->ManagedObject->pos.distanceTo(startPosition) < b->ManagedObject->pos.distanceTo(startPosition);
        });
    return result;
}

