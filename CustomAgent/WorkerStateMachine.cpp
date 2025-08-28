#include "WorkerStateMachine.h"

#include "../lux/annotate.hpp"
#include "Annotator.hpp"

#include "WorkerAI.h"

#include "Utils.hpp"
#include "Debug.h"

namespace WorkerSM
{
	DefaultState::DefaultState(WorkerSMInfos& stateInfos)
	{
		stateInfos.CurrentObjective = Objective::None;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> DefaultState::UpdateState(WorkerSMInfos& stateInfos)
	{
        Debug::Log("[SM_State] Update Default state");
        Debug::Log(WorkerSM::WorkerSMUtils::ObjectiveToString(stateInfos.CurrentObjective));
		switch (stateInfos.CurrentObjective)
		{
		case Objective::None:
			return nullptr;

		case Objective::CollectRessourceForSelf:
		case Objective::CollectRessourceForCity:
		{
			return std::move(CommonActions::GoCollectResources(stateInfos));
		}

		case Objective::BuildCity:
		{
			if (!stateInfos.ControlledWorker->CanBuildCity())
			{
				return std::move(CommonActions::GoCollectResources(stateInfos));
			}
			return std::move(CommonActions::GoBuildCity(stateInfos));

		}

		}

		return nullptr;
	}

	void DefaultState::DrawDebug(WorkerSMInfos& stateInfos)
	{
		stateInfos.Datas->AddAction(std::move(Annotate::text(stateInfos.TargetPosition.x + 1, stateInfos.TargetPosition.y + 1, "ST_D", 40)));
	}

	MovingState::MovingState(WorkerSMInfos& stateInfos, const Position& target)
	{
		stateInfos.TargetPosition = target;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> MovingState::UpdateState(WorkerSMInfos& stateInfos)
	{
        Debug::Log("[SM_State] Update Moving state");
		Unit* unit = stateInfos.ControlledWorker->ManagedObject;

		if (unit->pos == stateInfos.TargetPosition)
		{
			return std::move(NextState(stateInfos));
		}


		if (!unit->canAct())
		{
			return nullptr;
		}

		std::vector<DIRECTIONS> pathToTarget{};
		pathToTarget.reserve(10);

		bool pathFound = PathFinder::FindPath(stateInfos.Datas->Map, unit->pos, stateInfos.TargetPosition, stateInfos.Datas->Owner, pathToTarget);

		if (!pathFound)
		{
			return nullptr;
		}

		stateInfos.Datas->AddAction(std::move(unit->move(pathToTarget[0])));

		return nullptr;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> MovingState::NextState(WorkerSMInfos& stateInfos)
	{
		switch (stateInfos.CurrentObjective)
		{
		case Objective::BuildCity:
			if (stateInfos.ControlledWorker->CanBuildCity())
			{
				return std::unique_ptr<BuildingCityState>(new BuildingCityState());
			}
			return std::unique_ptr<CollectingRessourcesState>(new CollectingRessourcesState());

		case Objective::CollectRessourceForSelf:
			return std::unique_ptr<CollectingRessourcesState>(new CollectingRessourcesState());
		case Objective::CollectRessourceForCity:
			if (stateInfos.AlreadyCollectedResources)
			{
				return std::move(CommonActions::GoStandby(stateInfos));
			}
			return std::unique_ptr<CollectingRessourcesState>(new CollectingRessourcesState());

		}
		return nullptr;
	}

	void MovingState::DrawDebug(WorkerSMInfos& stateInfos)
	{
		Unit* unit = stateInfos.ControlledWorker->ManagedObject;

		std::vector<DIRECTIONS> pathToTarget{};
		pathToTarget.reserve(10);

		bool pathFound = PathFinder::FindPath(stateInfos.Datas->Map, unit->pos, stateInfos.TargetPosition, stateInfos.Datas->Owner, pathToTarget);

		Annotator::TracePath(unit->pos, pathToTarget, *stateInfos.Datas->Actions);

		stateInfos.Datas->AddAction(std::move(Annotate::x(stateInfos.TargetPosition.x, stateInfos.TargetPosition.y)));

		stateInfos.Datas->AddAction(std::move(Annotate::text(stateInfos.TargetPosition.x + 1, stateInfos.TargetPosition.y + 1, "ST_M", 40)));
	}

	std::unique_ptr<SMState<WorkerSMInfos>> BuildingCityState::UpdateState(WorkerSMInfos& stateInfos)
	{
        Debug::Log("[SM_State] Update BuildS state");

		Unit* unit = stateInfos.ControlledWorker->ManagedObject;
		if (!unit->canAct())
		{
			return nullptr;
		}

		if (!stateInfos.ControlledWorker->CanBuildCity())
		{
			return std::move(CommonActions::GoCollectResources(stateInfos));
		}

		Cell* cell = stateInfos.Datas->Map.getCell(unit->pos.x, unit->pos.y);
		if (cell->hasResource() || cell->citytile != nullptr)
		{
			return std::move(CommonActions::GoStandby(stateInfos));
		}

		stateInfos.Datas->AddAction(std::move(unit->buildCity()));

		return std::move(CommonActions::GoStandby(stateInfos));
	}

	void BuildingCityState::DrawDebug(WorkerSMInfos& stateInfos)
	{
		stateInfos.Datas->AddAction(std::move(Annotate::text(stateInfos.TargetPosition.x + 1, stateInfos.TargetPosition.y + 1, "ST_B", 40)));
	}

	std::unique_ptr<SMState<WorkerSMInfos>> CollectingRessourcesState::UpdateState(WorkerSMInfos& stateInfos)
	{
        Debug::Log("[SM_State] Update Collecting state");
		// If the city no longer needs resources
		if (stateInfos.CurrentObjective == Objective::CollectRessourceForCity && 
			!stateInfos.SuppliedCity->NeedResources(stateInfos.Datas->Turn))
		{
			stateInfos.AlreadyCollectedResources = true;
			return std::move(CommonActions::GoStandby(stateInfos));
		}

		Unit* unit = stateInfos.ControlledWorker->ManagedObject;

		if (unit->getCargoSpaceLeft() == 0)
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
			return std::move(CommonActions::GoStandby(stateInfos));

		case Objective::CollectRessourceForCity:
		{
			const CityTile* closestTile = PathFinder::GetClosestCityTile(stateInfos.ControlledWorker->ManagedObject->pos, stateInfos.SuppliedCity->ManagedObject, stateInfos.Datas->Map, stateInfos.Datas->Owner);
			return std::unique_ptr<MovingState>(new MovingState(stateInfos, closestTile->pos));
		}

		case Objective::BuildCity:
			return std::move(CommonActions::GoBuildCity(stateInfos));
		}
		
		return nullptr;
	}

	void CollectingRessourcesState::DrawDebug(WorkerSMInfos& stateInfos)
	{
		stateInfos.Datas->AddAction(std::move(Annotate::text(stateInfos.TargetPosition.x + 1, stateInfos.TargetPosition.y + 1, "ST_C", 40)));
	}

	std::unique_ptr<SMState<WorkerSMInfos>> CommonActions::GoStandby(WorkerSMInfos& stateInfos)
	{
		return std::unique_ptr<DefaultState>(new DefaultState(stateInfos));
	}

	std::unique_ptr<SMState<WorkerSMInfos>> CommonActions::GoBuildCity(WorkerSMInfos& stateInfos)
	{
		Debug::Log("[SM_State] Search for city cell");

		Cell* cityBuildTile = stateInfos.Datas->GetBestCityBuildingCell(stateInfos.ControlledWorker->ManagedObject->pos);
		if (cityBuildTile == nullptr)
		{
			Debug::LogError("No suitable tiles to build city");
			return std::unique_ptr<DefaultState>(new DefaultState(stateInfos));;
		}

		Debug::Log(Utils::FormatString("[SM_State] CityCell (%i, %i)", cityBuildTile->pos.x, cityBuildTile->pos.y));

		return std::unique_ptr<MovingState>(new MovingState(stateInfos, cityBuildTile->pos));
	}

	std::unique_ptr<SMState<WorkerSMInfos>> CommonActions::GoCollectResources(WorkerSMInfos& stateInfos)
	{
		Cell* targetCell = stateInfos.Datas->GetClosestResourceCell(stateInfos.ControlledWorker->ManagedObject->pos);
		Debug::Log(Utils::FormatString("[SM_State] ResourceCell (%i, %i)", targetCell->pos.x, targetCell->pos.y));
		return std::unique_ptr<MovingState>(new MovingState(stateInfos, targetCell->pos));
	}
}