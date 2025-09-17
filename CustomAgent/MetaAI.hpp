#pragma once
#include <vector>

#include "lux/city.hpp"
#include "lux/game_objects.hpp"

#include "lux/annotate.hpp"

#include "SubAIs/SubAI.h"
#include "SubAIs/Worker/WorkerAI.h"
#include "SubAIs/Cart/CartAI.hpp"
#include "SubAIs/City/CityAI.h"
#include "SubAIs/CityTile/CityTileAI.h"

#include "Utils/Debug/Debug.h"

/// <summary>
/// The main AI that manages the SubAIs' lifetime, give them orders, updates them and so on 
/// </summary>
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

	/// <summary>
	/// The proportion of city tiles that will create a unit. This can be used to speed up or down the AI expansion 
	/// </summary>
	static constexpr float UNIT_EXPANSION_FACTOR = 0.7f;

	int m_turn = 0;

	GameDatas* m_gameDatas;

	// Survive
	std::vector<std::unique_ptr<CityAI>> m_cityAIs;
	std::vector<CityTileAI> m_cityTileAIs;
	std::vector<std::unique_ptr<WorkerAI>> m_workerAIs;
	std::vector<std::unique_ptr<CartAI>> m_cartAIs;

public:
	MetaAI(GameDatas* gameDatas);

	void Update(int turn);

private:

	/// <summary>
	/// Creates and kill the subAIs if needed and relink all SubAIs to their managed object because they are destroyed for some reason
	/// </summary>
	void ManageAILives();

	void StartTurn();

	/// <summary>
	/// Recreates all the CityTileAIs because there is no good way to keep a reference to the cityTiles across multiple turn
	/// </summary>
	void CreateCityTilesAIs(std::vector<CityTile*>& allTiles);

	/// <summary>
	/// Give orders to all available SubAIs
	/// </summary>
	void GiveOrders();

	void UpdateSubAIs();

	void DrawDebug();

#pragma region Survive
	/// <summary>
	/// Instruct available worker to gather resources for the given city
	/// </summary>
	void MakeUnitsCollectResourcesForCity(CityAI& city);
	/// <summary>
	/// Instruct available worker to gather resources for themselves
	/// </summary>
	void MakeUnitsCollectResourcesForThemselves(WorkerAI& worker);
#pragma endregion

#pragma region Expand

	int NBUnitsToBuild() const;

	/// <summary>
	/// Instruct available cities to build new units
	/// </summary>
	void BuildUnits();

	/// <summary>
	/// Instruct available workers to build new city tiles
	/// </summary>
	void BuildCities();

	/// <summary>
	/// Instruct available cityTiles to research
	/// </summary>
	void Research();
#pragma endregion

#pragma region Utils
	/// <summary>
	/// Returns all the cities that need resources
	/// </summary>
	std::vector<CityAI*> GetNeedingCity();
	/// <summary>
	/// Returns all the workers that need resources
	/// </summary>
	std::vector<WorkerAI*> GetNeedingWorkers();
	/// <summary>
	/// Returns all the carts that need resources
	/// </summary>
	std::vector<CartAI*> GetNeedingCarts();

	/// <summary>
	/// Returns all the available workers sorted by their Manhattan distance to the given position
	/// </summary>
	std::vector<WorkerAI*> GetAvailableWorkersSortedByDistance(Position startPosition);
	/// <summary>
	/// Returns all the available carts sorted by their Manhattan distance to the given position
	/// </summary>
	std::vector<CartAI*> GetAvailableCartsSortedByDistance(Position startPosition);
#pragma endregion

	template<typename TSubAI>
	void ResetSubAIManagedObject(std::vector<TSubAI>& existingAIs)
	{
		for (TSubAI& subAI : existingAIs)
		{
			subAI.ManagedObject = nullptr;
		}
	}

	/// <summary>
	/// Cre
	/// </summary>
	/// <typeparam name="TSubAI"></typeparam>
	/// <typeparam name="TManagedObject"></typeparam>
	/// <param name="existingAIs"></param>
	/// <param name="existingObjects"></param>
	template<typename TSubAI, typename TManagedObject>
	void ManageSubAILife(std::vector<std::unique_ptr<TSubAI>>& existingAIs, std::vector<TManagedObject*>& existingObjects)
	{
		int existingObjectCount = existingAIs.size();

		std::vector<AILifeState> aiLifeStates = {};
		aiLifeStates.reserve(existingObjectCount);

		for (int i = 0; i < existingObjectCount; ++i)
		{
			aiLifeStates.emplace_back(i, false, existingAIs[i]->ManagedObjectID);
		}

		for (TManagedObject* object : existingObjects)
		{
			std::string objectID = GetID(object);
			int lifeStateIndex = 0;
			for (; lifeStateIndex < existingObjectCount; ++lifeStateIndex)
			{
				if (objectID == aiLifeStates[lifeStateIndex].ID)
				{
					break;
				}
			}

			if (lifeStateIndex == existingObjectCount) // If the object does not have a SubAI, creates one
			{
				EmplaceSubAI(existingAIs, object);
				continue;
			}

			aiLifeStates[lifeStateIndex].ShouldLive = true; // Sub AI should be kept alive
			TSubAI* ai = existingAIs[aiLifeStates[lifeStateIndex].Index].get();
			ai->ManagedObject = object; // Relink ai to its managed object

			Debug::LogWarning(Utils::FormatString("Relinking object with id \"%s\" (Ptr : %u) to AI (Ptr : %u)", GetID(object).c_str(), Utils::GetPtrRepr(object), Utils::GetPtrRepr(ai)));
		}

		// Delete dead sub AI
		int deletedItemsCount = 0;
		for (AILifeState& lifeState : aiLifeStates)
		{
			if (lifeState.ShouldLive)
			{
				Debug::LogWarning(Utils::FormatString("[MetaAI] Kept alive Sub AI with ID : %s", lifeState.ID.c_str()));
				continue;
			}

			int index = lifeState.Index - deletedItemsCount;
			Debug::LogWarning(Utils::FormatString("[MetaAI] Deleted Sub AI with ID : %s | at index %i/%i", lifeState.ID.c_str(), index, existingAIs.size()));

			existingAIs.erase(existingAIs.begin() + index);
			deletedItemsCount++;

			Debug::LogWarning("[MetaAI] Deletion successful");
		}
	}

	void EmplaceSubAI(std::vector<std::unique_ptr<WorkerAI>>& targetVector, Unit* managedObject)
	{
		targetVector.emplace_back(new WorkerAI(managedObject, m_gameDatas));
		Debug::LogWarning(Utils::FormatString("Creating new [Worker] in (%i, %i) (Ptr : %u)", managedObject->pos.x, managedObject->pos.y, Utils::GetPtrRepr(managedObject)));
	}

	void EmplaceSubAI(std::vector<std::unique_ptr<CartAI>>& targetVector, Unit* managedObject)
	{
		targetVector.emplace_back(new CartAI(managedObject, m_gameDatas));
		Debug::LogWarning(Utils::FormatString("Creating new [Cart] in (%i, %i) (Ptr : %u)", managedObject->pos.x, managedObject->pos.y, Utils::GetPtrRepr(managedObject)));
	}

	void EmplaceSubAI(std::vector<std::unique_ptr<CityAI>>& targetVector, City* managedObject)
	{
		targetVector.emplace_back(new CityAI(managedObject, m_gameDatas));
		Debug::LogWarning(Utils::FormatString("Creating new [City] in (%i, %i) (Ptr : %u)", managedObject->citytiles[0].pos, managedObject->citytiles[0].pos, Utils::GetPtrRepr(managedObject)));
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
