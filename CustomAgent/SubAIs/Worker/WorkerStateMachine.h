#pragma once
#include <memory>

#include "lux/game_objects.hpp"
#include "lux/map.hpp"

#include "Utils/Alias.h"

#include "StateMachine/SMState.h"
#include "SubAIs/Worker/WorkerObjective.hpp"
#include "Utils/PathFinder.hpp"
#include "GameDatas.h"
#include "SubAIs/City/CityAI.h"

class WorkerAI;

namespace WorkerSM 
{
	using namespace lux;


	class WorkerSMUtils
	{
	public:
		static string ObjectiveToString(Objective obj)
		{
			switch (obj)
			{
				case Objective::None:
					return "None";
				case Objective::BuildCity:
					return "Build";
				case Objective::CollectResourceForSelf:
					return "Res_S";
				case Objective::CollectResourceForCity:
					return "Res_C";
			}
			return "INV";
		}
	};

	/// <summary>
	/// A collection of all the datas the Worker's state machine will need
	/// </summary>
	struct WorkerSMInfos 
	{
		GameDatas* Datas;
		WorkerAI* ControlledWorker;
		Objective CurrentObjective;

		std::string SuppliedCityID;
		bool AlreadyCollectedResources;

		Position cityTargetPosition;

		Position TargetPosition;
		PathFindingFlags PathOptions;


		WorkerSMInfos(WorkerAI* worker, GameDatas* datas) :
			ControlledWorker(worker),
			Datas(datas),
			CurrentObjective(Objective::None),
			SuppliedCityID(""),
			AlreadyCollectedResources(false),
			TargetPosition(-1, -1),
			PathOptions(PathFindingFlags::None)
		{

		}

		/// <summary>
		/// Return the CityAI associated with the stored SuppliedCityID if it's still alive, nullptr otherwise
		/// </summary>
		CityAI* GetSuppliedCity()
		{
			if (SuppliedCityID == "")
			{
				return nullptr;
			}

			for (int i = 0; i < Datas->CityAIs->size(); ++i)
			{
				CityAI* ai = (*Datas->CityAIs)[i].get();
				if (ai->ManagedObjectID == SuppliedCityID)
				{
					return ai;
				}
			}

			return nullptr;
		}
	};

	/// <summary>
	/// The default state of worker in which it doesn't do anything but try to change state depending on its objective
	/// </summary>
	class DefaultState : public SMState<WorkerSMInfos>
	{
	public:

		DefaultState(WorkerSMInfos& stateInfos);

		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;

		virtual void DrawDebug(WorkerSMInfos& stateInfos) override;
	};

	/// <summary>
	/// The state in which the worker is when moving to an other location
	/// </summary>
	class MovingState : public SMState<WorkerSMInfos>
	{
	private:
		Path m_path;
		Position m_lastPosition;

	public:
		MovingState(WorkerSMInfos& stateInfos, const Position& target, PathFindingFlags pathOptions = PathFindingFlags::None);

		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;

		std::unique_ptr<SMState<WorkerSMInfos>> NextState(WorkerSMInfos& stateInfos);

		virtual void DrawDebug(WorkerSMInfos& stateInfos) override;
	};

	/// <summary>
	/// The state in which the worker is when building a city
	/// </summary>
	class BuildingCityState : public SMState<WorkerSMInfos>
	{
		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;
		virtual void DrawDebug(WorkerSMInfos& stateInfos) override;
	};

	/// <summary>
	/// The state in which the worker is when gathering resources  
	/// </summary>
	class CollectingResourcesState : public SMState<WorkerSMInfos>
	{
		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;

		std::unique_ptr<SMState<WorkerSMInfos>> NextState(WorkerSMInfos& stateInfos);
		virtual void DrawDebug(WorkerSMInfos& stateInfos) override;

		bool ShouldGoNextState(WorkerSMInfos& stateInfos);
	};
	
	/// <summary>
	/// Helper class with common actions the worker can make
	/// </summary>
	class CommonActions
	{
	public:
		/// <summary>
		/// Puts the worker in a standby mode, awaiting new orders from the MetaAI
		/// </summary>
		static std::unique_ptr<SMState<WorkerSMInfos>> GoStandby(WorkerSMInfos& stateInfos);
		/// <summary>
		/// Tells the worker to pick a tile to build a city tile on if it has enough resources, or to go gather resources otherwise
		/// </summary>
		static std::unique_ptr<SMState<WorkerSMInfos>> GoBuildCity(WorkerSMInfos& stateInfos);
		/// <summary>
		/// Tells the worker to go to the closest resource tile to gather resources 
		/// </summary>
		static std::unique_ptr<SMState<WorkerSMInfos>> GoCollectResources(WorkerSMInfos& stateInfos);
	};
}

