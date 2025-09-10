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
        std::string ID;

        AILifeState(int index, bool shouldLive, std::string id) :
            Index(index),
            ShouldLive(shouldLive),
            ID(std::move(id))
        {
        }
    };

    static constexpr int RESOURCE_PER_UNIT = 100;

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
    void StartTurn();
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
        int existingObjectCount = existingAIs.size();

        std::vector<AILifeState> aiLifeStates = {};
        aiLifeStates.reserve(existingObjectCount);

        for (int i = 0; i < existingObjectCount; ++i)
        {
            aiLifeStates.emplace_back(i, false, existingAIs[i].ManagedObjectID);
        }

        for (TManagedObject* object : existingObjects)
        {
            std::string objectID = GetID(object);
            int lifeStateIndex = 0;
            for ( ;lifeStateIndex < existingObjectCount; ++lifeStateIndex)
            {
                if (objectID == aiLifeStates[lifeStateIndex].ID)
                {
                    break;
                }
            }

            if (lifeStateIndex == existingObjectCount) // New sub AI
            {
                EmplaceSubAI(existingAIs, object);
                continue;
            }

            aiLifeStates[lifeStateIndex].ShouldLive = true; // Sub AI should be kept alive
            TSubAI* ai = &existingAIs[aiLifeStates[lifeStateIndex].Index];
            ai->ManagedObject = object; // Relink ai to its managed object
            
            Debug::LogWarning(Utils::FormatString("Relinking object with id \"%s\" (Ptr : %ld) to AI (Ptr : %ld)", GetID(object).c_str(), (long) object, (long)ai));
        }

        // Delete dead sub AI
        int deletedItemsCount = 0;
        for (AILifeState& lifeState : aiLifeStates)
        {
            if (lifeState.ShouldLive)
            {
                Debug::LogWarning(Utils::FormatString("[MetaAI] Deleted Sub AI with ID : %s", lifeState.ID.c_str()));
                continue;
            }

            int index = lifeState.Index - deletedItemsCount;
            Debug::LogWarning(Utils::FormatString("[MetaAI] Deleted Sub AI with ID : %s | at index %i/%i", lifeState.ID.c_str(), index, existingAIs.size()));

            existingAIs.erase(existingAIs.begin() + index);
            deletedItemsCount++;

            Debug::LogWarning("[MetaAI] Deletion successful");
        }
    }

    void EmplaceSubAI(std::vector<WorkerAI>& targetVector, Unit* managedObject)
    {
        targetVector.emplace_back(managedObject, m_gameDatas);
        Debug::LogWarning(Utils::FormatString("Creating new [Worker] in (%i, %i) (Ptr : %ld)", managedObject->pos.x, managedObject->pos.y, (long) managedObject));
    }

    void EmplaceSubAI(std::vector<CartAI>& targetVector, Unit* managedObject)
    {
        targetVector.emplace_back(managedObject, m_gameDatas);
        Debug::LogWarning(Utils::FormatString("Creating new [Cart] in (%i, %i) (Ptr : %ld)", managedObject->pos.x, managedObject->pos.y, (long) managedObject));
    }

    void EmplaceSubAI(std::vector<CityAI>& targetVector, City* managedObject)
    {
        targetVector.emplace_back(managedObject, m_gameDatas);
        Debug::LogWarning(Utils::FormatString("Creating new [City] in (%i, %i) (Ptr : %ld)", managedObject->citytiles[0].pos, managedObject->citytiles[0].pos, (long) managedObject));
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
