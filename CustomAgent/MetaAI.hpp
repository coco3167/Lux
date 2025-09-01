#pragma once
#include <vector>

#include "../lux/city.hpp"
#include "../lux/game_objects.hpp"

#include "../lux/annotate.hpp"

#include "CityAI.h"
#include "CityTileAI.h"
#include "SubAI.h"
#include "WorkerAI.h"
#include "CartAI.hpp"

#include "Debug.h"

class MetaAI
{
    /*
    *  Goals one after the other
    *  - Survive (keep units and city alive through each night)
    *  - Expand (more cities, more units, roads, research)
    *  - Attack (go annoy the ennemy, not necessary)
    */
private:
    struct AILifeState
    {
        int Index;
        bool ShouldLive;

        AILifeState(int index, bool shouldLive) :
            Index(index),
            ShouldLive(shouldLive)
        {
        }
    };

    static constexpr int RESOURCE_PER_UNIT = 10;

    int m_turn = 0;

    GameDatas* m_gameDatas;

    // Survive
    std::vector<CityAI> m_cityAIs;
    std::vector<CityTileAI> m_cityTileAIs;
    std::vector<WorkerAI> m_workerAIs;
    std::vector<CartAI> m_cartAIs;

public:
    MetaAI(GameDatas* gameDatas);

    void Update(int turn);

private:
    void ManageAILives();
    void CreateCityTilesAIs(std::vector<CityTile*>& allTiles);
    void GiveOrders();
    void UpdateSubAIs();
    void DrawDebug();

    //Expand
    int NBUnitsToBuild() const;
    void MakeUnitsCollectResourcesForCity(CityAI& city);
    void MakeUnitsCollectResourcesForThemselves(WorkerAI& worker);
    void BuildUnits();
    void BuildCities();
    void Research();

    std::vector<CityAI*> GetNeedingCity();
    std::vector<WorkerAI*> GetNeedingWorkers();
    std::vector<CartAI*> GetNeedingCarts();

    std::vector<WorkerAI*> GetAvailableWorkersSortedByDistance(Position startPosition);
    std::vector<CartAI*> GetAvailableCartsSortedByDistance(Position startPosition);

    template<typename TSubAI>
    void ResetSubAIManagedObject(std::vector<TSubAI>& existingAIs)
    {
        for (TSubAI& subAI : existingAIs)
        {
            subAI.ManagedObject = nullptr;
        }
    }

    template<typename TSubAI, typename TManagedObject>
    void ManageSubAILife(std::vector<TSubAI>& existingAIs, std::vector<TManagedObject*>& existingObjects)
    {
        std::unordered_map<std::string, AILifeState> aiLifeStates = {};
        aiLifeStates.reserve(existingAIs.size());

        for (int i = 0; i < existingAIs.size(); ++i)
        {
            aiLifeStates.insert({ existingAIs[i].ManagedObjectID, AILifeState{i, false} });
        }

        for (TManagedObject* object : existingObjects)
        {
            auto objectIterator = aiLifeStates.find(GetID(object));

            if (objectIterator == aiLifeStates.end()) // New sub AI
            {
                EmplaceSubAI(existingAIs, object);
                Debug::LogWarning("Adding 1 sub ai");
                continue;
            }

            objectIterator->second.ShouldLive = true; // Sub AI should be kept alive
            existingAIs[objectIterator->second.Index].ManagedObject = object; // Relink ai to its managed object
        }

        // Delete dead sub AI
        int deletedItemsCount = 0;
        for (auto& lifeState : aiLifeStates)
        {
            if (lifeState.second.ShouldLive)
            {
                continue;
            }

            Debug::LogWarning("Deleted 1 sub ai");

            existingAIs.erase(existingAIs.begin() + lifeState.second.Index - deletedItemsCount);
            deletedItemsCount++;
        }
    }

    void EmplaceSubAI(std::vector<WorkerAI>& targetVector, Unit* managedObject)
    {
        targetVector.emplace_back(managedObject, m_gameDatas);
    }

    void EmplaceSubAI(std::vector<CartAI>& targetVector, Unit* managedObject)
    {
        targetVector.emplace_back(managedObject, m_gameDatas);
    }

    void EmplaceSubAI(std::vector<CityAI>& targetVector, City* managedObject)
    {
        targetVector.emplace_back(managedObject, m_gameDatas);
    }

    string GetID(Unit* unit)
    {
        return unit->id;
    }

    string GetID(City* city)
    {
        return city->cityid;
    }
};
