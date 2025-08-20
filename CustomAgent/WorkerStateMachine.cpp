#include "WorkerStateMachine.h"

namespace WorkerSM
{
	DefaultState::DefaultState(WorkerSMInfos& stateInfos)
	{
		stateInfos.CurrentObjective = Objective::None;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> DefaultState::UpdateState(WorkerSMInfos& stateInfos)
	{
		switch (stateInfos.CurrentObjective)
		{
		case Objective::None:
			return nullptr;

		case Objective::CollectRessourceForSelf:
		case Objective::CollectRessourceForCity:
			Cell* targetCell = stateInfos.Datas.GetClosestResourceCell(stateInfos.ControlledWorker.pos);
			return std::unique_ptr<MovingState>(new MovingState(stateInfos, targetCell->pos));

		case Objective::BuildCity:
			Cell* cityBuildTile = stateInfos.Datas.GetBestCityBuildingCell(stateInfos.ControlledWorker.pos);
			return std::unique_ptr<MovingState>(new MovingState(stateInfos, cityBuildTile->pos));

		}
	}

	MovingState::MovingState(WorkerSMInfos& stateInfos, const Position& target)
	{
		stateInfos.TargetPosition = target;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> MovingState::UpdateState(WorkerSMInfos& stateInfos)
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

	std::unique_ptr<SMState<WorkerSMInfos>> MovingState::NextState(WorkerSMInfos& stateInfos)
	{
		switch (stateInfos.CurrentObjective)
		{
		case Objective::BuildCity:
			return std::unique_ptr<BuildingCityState>(new BuildingCityState());

		case Objective::CollectRessourceForSelf:
		case Objective::CollectRessourceForCity:
			return std::unique_ptr<CollectingRessourcesState>(new CollectingRessourcesState());

		}
	}

	std::unique_ptr<SMState<WorkerSMInfos>> BuildingCityState::UpdateState(WorkerSMInfos& stateInfos)
	{
		Unit& unit = stateInfos.ControlledWorker;
		if (!unit.canAct())
		{
			return nullptr;
		}

		stateInfos.Datas.AddAction(std::move(unit.buildCity()));

		return std::unique_ptr<DefaultState>(new DefaultState(stateInfos));
	}

	std::unique_ptr<SMState<WorkerSMInfos>> CollectingRessourcesState::UpdateState(WorkerSMInfos& stateInfos)
	{
		Unit& unit = stateInfos.ControlledWorker;

		if (unit.getCargoSpaceLeft() == 0)
		{
			return std::move(NextState(stateInfos));
		}

		return nullptr;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> CollectingRessourcesState::NextState(WorkerSMInfos& stateInfos)
	{
		switch (stateInfos.CurrentObjective)
		{
		case Objective::CollectRessourceForSelf:
			return std::unique_ptr<DefaultState>(new DefaultState(stateInfos));

		case Objective::CollectRessourceForCity:
			const CityTile* closestTile = PathFinder::GetClosestCityTile(stateInfos.ControlledWorker.pos, stateInfos.SuppliedCity, stateInfos.Datas.Map, stateInfos.Datas.Owner);
			return std::unique_ptr<MovingState>(new MovingState(stateInfos, closestTile->pos));
		}
	}
}