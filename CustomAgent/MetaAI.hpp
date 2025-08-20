#pragma once
#include <vector>

#include "../lux/city.hpp"
#include "../lux/game_objects.hpp"

#include "CityAI.h"
#include "SubAI.h"
#include "WorkerAI.h"

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

    GameDatas& m_gameDatas;

    // Survive
    std::vector<CityAI> m_cityAIs;
    std::vector<WorkerAI> m_workerAIs;
    std::vector<WorkerAI> m_cartAIs;

public:
    MetaAI(GameDatas& gameDatas);

    void Update(int turn);

private:

    void ManageAILives();
    void GiveOrders();
    void UpdateSubAIs();

    std::vector<CityAI&> GetNeedingCity() const;
    std::vector<WorkerAI&> GetNeedingUnits() const;

    //Expand
    int NBUnitsToBuild() const;
    void MakeUnitsCollectResourcesForCity(CityAI& city);
    void MakeUnitsCollectResourcesForThemselves(WorkerAI& unit);
    void BuildUnits();
    void BuildCities();
    void Research();

    template<typename TSubAI, typename TManagedObject>
    void ManageSubAILife(std::vector<TSubAI>& existingAIs, std::vector<TManagedObject&>& existingObjects)
    {
        std::unordered_map<TManagedObject&, AILifeState> aiLifeStates = {};
        aiLifeStates.reserve(existingAIs.size());

        for (int i = 0; i < existingAIs.size(); ++i)
        {
            aiLifeStates.insert({ existingAIs[i].ManagedObject, AILifeState{i, false} });
        }

        for (TManagedObject& object : existingObjects)
        {
            auto objectIterator = aiLifeStates.find(object);

            if (objectIterator == aiLifeStates.end()) // New sub AI
            {
                EmplaceSubAI(existingAIs, object);
                continue;
            }

            objectIterator->second.ShouldLive = true; // Sub AI should be kept alive
        }

        // Delete dead sub AI
        int deletedItemsCount = 0;
        for (auto& lifeState : aiLifeStates)
        {
            if (lifeState.second.ShouldLive)
            {
                continue;
            }

            existingAIs.erase(existingAIs.begin() + lifeState.second.Index - deletedItemsCount++);
        }
    }

    template<typename TSubAI, typename TManagedObject>
    void EmplaceSubAI(std::vector<TSubAI>& targetVector, TManagedObject& managedObject)
    {
    }

    template<>
    void EmplaceSubAI<WorkerAI, Unit>(std::vector<WorkerAI>& targetVector, Unit& managedObject)
    {
        targetVector.emplace_back(managedObject, m_gameDatas);
    }

    template<>
    void EmplaceSubAI<CityAI, City>(std::vector<CityAI>& targetVector, City& managedObject)
    {
        targetVector.emplace_back(managedObject);
    }

};
