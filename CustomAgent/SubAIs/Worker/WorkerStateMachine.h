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
				case Objective::CollectRessourceForSelf:
					return "Res_S";
				case Objective::CollectRessourceForCity:
					return "Res_C";
			}
			return "INV";
		}
	};

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

	class DefaultState : public SMState<WorkerSMInfos>
	{
	public:

		DefaultState(WorkerSMInfos& stateInfos);

		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;

		virtual void DrawDebug(WorkerSMInfos& stateInfos) override;
	};

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

	class BuildingCityState : public SMState<WorkerSMInfos>
	{
		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;
		virtual void DrawDebug(WorkerSMInfos& stateInfos) override;
	};

	class CollectingRessourcesState : public SMState<WorkerSMInfos>
	{
		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;

		std::unique_ptr<SMState<WorkerSMInfos>> NextState(WorkerSMInfos& stateInfos);
		virtual void DrawDebug(WorkerSMInfos& stateInfos) override;

		bool ShouldGoNextState(WorkerSMInfos& stateInfos);
	};

	class CommonActions
	{
	public:
		static std::unique_ptr<SMState<WorkerSMInfos>> GoStandby(WorkerSMInfos& stateInfos);
		static std::unique_ptr<SMState<WorkerSMInfos>> GoBuildCity(WorkerSMInfos& stateInfos);
		static std::unique_ptr<SMState<WorkerSMInfos>> GoCollectResources(WorkerSMInfos& stateInfos);
	};
}

