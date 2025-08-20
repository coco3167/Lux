#pragma once
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

	struct WorkerSMInfos 
	{
		GameDatas& Datas;
		Unit& ControlledWorker;
		Objective CurrentObjective;

		City* SuppliedCity;

		Position TargetPosition;


		WorkerSMInfos(Unit& worker, GameDatas& datas) :
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

		DefaultState(WorkerSMInfos& stateInfos) 
		{
			stateInfos.CurrentObjective = Objective::None;
		}

		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override
		{
			switch (stateInfos.CurrentObjective) 
			{
				case Objective::None:
					return nullptr;

				case Objective::CollectRessourceForSelf:
				case Objective::CollectRessourceForCity:
					Cell* targetCell = stateInfos.Datas.GetClosestResourceCell(stateInfos.ControlledWorker.pos);
					return std::make_unique<MovingState>(stateInfos, targetCell->pos);

				case Objective::BuildCity:
					Cell* cityBuildTile = stateInfos.Datas.GetBestCityBuildingCell(stateInfos.ControlledWorker.pos);
					return std::make_unique<MovingState>(stateInfos, cityBuildTile->pos);

			}
		}
	};

	class MovingState : public SMState<WorkerSMInfos>
	{
	public:
		MovingState(WorkerSMInfos& stateInfos, const Position& target)
		{
			stateInfos.TargetPosition = target;
		}

		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override
		{
			Unit& unit = stateInfos.ControlledWorker;

			if (unit.pos == stateInfos.TargetPosition) 
			{
				return std::move(NextState(stateInfos));
			}


			if (!unit.canAct())
			{
				return nullptr;
			}

			std::vector<DIRECTIONS> pathToTarget{};
			pathToTarget.reserve(10);

			bool pathFound = PathFinder::FindPath(stateInfos.Datas.Map, unit.pos, stateInfos.TargetPosition, stateInfos.Datas.Owner, pathToTarget);

			if (!pathFound) 
			{
				return nullptr;
			}

			stateInfos.Datas.AddAction(std::move(unit.move(pathToTarget[0])));

			return nullptr;
		}

		std::unique_ptr<SMState<WorkerSMInfos>> NextState(WorkerSMInfos& stateInfos) 
		{
			switch (stateInfos.CurrentObjective) 
			{
				case Objective::BuildCity:
					return std::make_unique<BuildingCityState>();

				case Objective::CollectRessourceForSelf:
				case Objective::CollectRessourceForCity:
					return std::make_unique<CollectingRessourcesState>();

			}
		}
	};

	class BuildingCityState : public SMState<WorkerSMInfos>
	{
		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override
		{
			Unit& unit = stateInfos.ControlledWorker;
			if (!unit.canAct())
			{
				return nullptr;
			}

			stateInfos.Datas.AddAction(std::move(unit.buildCity()));

			return std::make_unique<DefaultState>(stateInfos);
		}
	};

	class CollectingRessourcesState : public SMState<WorkerSMInfos>
	{
		virtual std::unique_ptr<SMState<WorkerSMInfos>> UpdateState(WorkerSMInfos& stateInfos) override
		{
			Unit& unit = stateInfos.ControlledWorker;

			if (unit.getCargoSpaceLeft() == 0) 
			{
				return std::move(NextState(stateInfos));
			}

			return nullptr;
		}

		std::unique_ptr<SMState<WorkerSMInfos>> NextState(WorkerSMInfos& stateInfos)
		{
			switch (stateInfos.CurrentObjective)
			{
			case Objective::CollectRessourceForSelf:
				return std::make_unique<DefaultState>(stateInfos);

			case Objective::CollectRessourceForCity:
				const CityTile* closestTile = Utils::GetClosestCityTile(stateInfos.ControlledWorker.pos, stateInfos.SuppliedCity, stateInfos.Datas.Map, stateInfos.Datas.Owner);
				return std::make_unique<MovingState>(stateInfos, closestTile->pos);
			}
		}
	};
}

