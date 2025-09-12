#include "MetaAI.hpp"

#include <algorithm>

#include "Utils/Debug/Debug.h"


MetaAI::MetaAI(GameDatas* gameDatas):
    m_gameDatas(gameDatas)
{
    m_gameDatas->WorkerAIs = &m_workerAIs;
    m_gameDatas->CartAIs = &m_cartAIs;
    m_gameDatas->CityAIs = &m_cityAIs;

    m_workerAIs.reserve(512);
    m_cartAIs.reserve(512);
    m_cityAIs.reserve(512);
}

// Other
void MetaAI::Update(const int turn)
{
    m_turn = turn;

    Debug::Log("ManageLives");
    ManageAILives();

    Debug::Log("StartTurn");
    StartTurn();

    Debug::Log("GiveOrders");
    GiveOrders();

    Debug::Log("Update SubAis");
    UpdateSubAIs();

    Debug::Log("Debug");
    DrawDebug();
}

void MetaAI::ManageAILives()
{
    //ResetSubAIManagedObject(m_workerAIs);
    //ResetSubAIManagedObject(m_cartAIs);
    //ResetSubAIManagedObject(m_cityAIs);

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
    
    Debug::LogWarning(Utils::FormatString("Worker vector Ptr : %ld", (long)&m_workerAIs));

    for (int i = 0; i < m_workerAIs.size(); ++i)
    {
        Debug::LogWarning(Utils::FormatString("Worker Ptr : %ld", (long)m_workerAIs[i].get()));
    }

    Debug::Log("[MetaAI] Manage Worker Lives");
    ManageSubAILife<WorkerAI, lux::Unit>(m_workerAIs, workers);
    Debug::Log("[MetaAI] Manage Carts Lives");
    ManageSubAILife<CartAI, lux::Unit>(m_cartAIs, carts);
    Debug::Log("[MetaAI] Manage Cities Lives");
    ManageSubAILife<CityAI, lux::City>(m_cityAIs, cities);
    Debug::Log("[MetaAI] Manage CityTiles Lives");
    CreateCityTilesAIs(cityTiles);
}

void MetaAI::StartTurn()
{
    for (int i = 0; i < m_workerAIs.size(); ++i)
    {
        m_workerAIs[i]->BeginTurn();
    }

    for (int i = 0; i < m_cartAIs.size(); ++i)
    {
        m_cartAIs[i]->BeginTurn();
    }
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
    Debug::LogWarning(Utils::FormatString("Worker vector Ptr : %ld", (long) &m_workerAIs));

    for (int i = 0; i < m_workerAIs.size(); ++i)
    {
        Debug::LogWarning(Utils::FormatString("Worker Ptr : %ld", (long)m_workerAIs[i].get()));
        m_workerAIs[i]->Update();
    }

    Debug::Log("Update Carts");

    for (int i = 0; i < m_cartAIs.size(); ++i)
    {
        m_cartAIs[i]->Update();
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

    for (int i = 0; i < m_workerAIs.size(); ++i)
    {
        m_workerAIs[i]->DrawDebug(*m_gameDatas);
    }

    for (int i = 0; i < m_cartAIs.size(); ++i)
    {
        m_cartAIs[i]->DrawDebug(*m_gameDatas);
    }

    for (int i = 0; i < m_cityAIs.size(); ++i)
    {
        m_cityAIs[i]->DrawDebug(*m_gameDatas);
    }
}

// Survive
std::vector<CityAI*> MetaAI::GetNeedingCity()
{
    // return all cities that need to be given resources
    std::vector<CityAI*> needyCity;
    needyCity.reserve(m_cityAIs.size());

    for (int i = 0; i < m_cityAIs.size(); ++i)
    {
        if (m_cityAIs[i]->NeedResources(m_turn))
        {
            needyCity.push_back(m_cityAIs[i].get());
        }
    }
    return needyCity;
}

std::vector<WorkerAI*> MetaAI::GetNeedingWorkers()
{
    // return all Units that need resources
    std::vector<WorkerAI*> needyWorkers;
    needyWorkers.reserve(m_workerAIs.size());

    for (int i = 0; i < m_workerAIs.size(); ++i)
    {
        if (m_workerAIs[i]->NeedResources(m_turn))
        {
            needyWorkers.push_back(m_workerAIs[i].get());
        }
    }
    return needyWorkers;
}

std::vector<CartAI*> MetaAI::GetNeedingCarts()
{
    std::vector<CartAI*> needyCarts;
    needyCarts.reserve(m_cartAIs.size());

    for (int i = 0; i < m_cartAIs.size(); ++i)
    {
        if (m_cartAIs[i]->NeedResources(m_turn))
        {
            needyCarts.push_back(m_cartAIs[i].get());
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
    return static_cast<int>(std::ceil(m_cityTileAIs.size() * 0.8f - m_gameDatas->Owner->units.size()));
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
    if (m_gameDatas->TurnsUntilNight() < 10)
    {
        return;
    }
    for (int i = 0; i < m_workerAIs.size(); ++i)
    {
        if(m_workerAIs[i]->IsAvailable())
        {
            m_workerAIs[i]->BuildCityTile();
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

    for (int i = 0; i < m_workerAIs.size(); ++i)
    {
        if (!m_workerAIs[i]->IsAvailable())
        {
            continue;
        }
        result.push_back(m_workerAIs[i].get());
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

    for (int i = 0; i < m_cartAIs.size(); ++i)
    {
        if (!m_cartAIs[i]->IsAvailable())
        {
            continue;
        }
        result.push_back(m_cartAIs[i].get());
    }

    std::sort(result.begin(), result.end(),
        [startPosition](CartAI* a, CartAI* b)
        {
            return a->ManagedObject->pos.distanceTo(startPosition) < b->ManagedObject->pos.distanceTo(startPosition);
        });
    return result;
}

