#pragma once
#include <memory>

#include "StateMachine/SMState.h"
#include "PathFinder.hpp"
#include "GameDatas.h"

#include "../lux/game_objects.hpp"
#include "../lux/map.hpp"



namespace WorkerSM 
{
	using namespace lux;

	enum class Objective 
	{
		None,
		BuildCity,
		CollectRessourceForSelf,
		CollectRessourceForCity,
	};

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
		Unit* ControlledWorker;
		Objective CurrentObjective;

		City* SuppliedCity;

		Position TargetPosition;


		WorkerSMInfos(Unit* worker, GameDatas* datas) :
			ControlledWorker(worker),
			Datas(datas),
			CurrentObjective(Objective::None),
			SuppliedCity(nullptr),
			TargetPosition(-1, -1)
		{

		}
	};

	class DefaultState : public SMState<WorkerSMInfos>
	{
	public:

		DefaultState(WorkerSMInfos& stateInfos);

		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;
	};

	class MovingState : public SMState<WorkerSMInfos>
	{
	public:
		MovingState(WorkerSMInfos& stateInfos, const Position& target);

		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;

		std::unique_ptr<SMState<WorkerSMInfos>> NextState(WorkerSMInfos& stateInfos);


		virtual void DrawDebug(std::vector<std::string>& actions) override;
	};

	class BuildingCityState : public SMState<WorkerSMInfos>
	{
		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;
	};

	class CollectingRessourcesState : public SMState<WorkerSMInfos>
	{
		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override;

		std::unique_ptr<SMState<WorkerSMInfos>> NextState(WorkerSMInfos& stateInfos);
	};
}

