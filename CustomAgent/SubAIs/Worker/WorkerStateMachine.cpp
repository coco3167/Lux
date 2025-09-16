#include "WorkerStateMachine.h"

#include "lux/annotate.hpp"
#include "Utils/Debug/Annotator.hpp"

#include "SubAIs/Worker/WorkerAI.h"

#include "Utils/Utils.hpp"
#include "Utils/Debug/Debug.h"

namespace WorkerSM
{
	DefaultState::DefaultState(WorkerSMInfos& stateInfos)
	{
		stateInfos.CurrentObjective = Objective::None;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> DefaultState::UpdateState(WorkerSMInfos& stateInfos)
	{
        Debug::Log("[SM_DefaultState] Update");
        Debug::Log(WorkerSM::WorkerSMUtils::ObjectiveToString(stateInfos.CurrentObjective));
		// Does nothing but try to change state depending on the current objective
		switch (stateInfos.CurrentObjective)
		{
		case Objective::None:
			return nullptr;

		case Objective::CollectResourceForSelf:
		case Objective::CollectResourceForCity:
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

	MovingState::MovingState(WorkerSMInfos& stateInfos, const Position& target, PathFindingFlags pathOptions) :
		m_lastPosition(-1, -1)
	{
		m_path.reserve(10);

		stateInfos.TargetPosition = target;
		stateInfos.PathOptions = pathOptions;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> MovingState::UpdateState(WorkerSMInfos& stateInfos)
	{
        Debug::Log("[SM_MovingState] Update");
		Unit* unit = stateInfos.ControlledWorker->ManagedObject;

		if (unit == nullptr)
		{
			Debug::LogError("[SM_MovingState] Worker's managedObject is null");
			return std::move(CommonActions::GoStandby(stateInfos));
		}

		Position unitPos = unit->pos;
		Position targetPos = stateInfos.TargetPosition;

		if (unit->pos == stateInfos.TargetPosition)
		{
			Debug::Log("[SM_MovingState] Next state");
			return std::move(NextState(stateInfos));
		}

		if (!unit->canAct())
		{
			return nullptr;
		}

		Debug::Log(Utils::FormatString("[SM_MovingState] Compute path (Path options : %i)", (int)stateInfos.PathOptions));

		bool pathFound = PathFinder::FindPath(*stateInfos.Datas, unit->pos, stateInfos.TargetPosition, m_path, stateInfos.PathOptions);

		if (!pathFound)
		{
			return std::move(CommonActions::GoStandby(stateInfos));
		}

		Debug::LogWarning(Utils::FormatString("[SM_MovingState] Path Length %i", m_path.size()));
		stateInfos.ControlledWorker->MoveUnit(stateInfos.Datas, m_path[0]);
		m_lastPosition = unit->pos;

		return nullptr;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> MovingState::NextState(WorkerSMInfos& stateInfos)
	{
		switch (stateInfos.CurrentObjective)
		{
		case Objective::BuildCity:
			if (stateInfos.ControlledWorker->CanBuildCity())
			{
				if (stateInfos.cityTargetPosition == stateInfos.TargetPosition)
				{
					return std::unique_ptr<BuildingCityState>(new BuildingCityState());
				}
				return std::move(CommonActions::GoBuildCity(stateInfos));
			}
			return std::move(CommonActions::GoCollectResources(stateInfos));

		case Objective::CollectResourceForSelf:
			return std::unique_ptr<CollectingResourcesState>(new CollectingResourcesState());
		case Objective::CollectResourceForCity:
			if (stateInfos.AlreadyCollectedResources)
			{
				return std::move(CommonActions::GoStandby(stateInfos));
			}
			return std::unique_ptr<CollectingResourcesState>(new CollectingResourcesState());

		}
		return nullptr;
	}

	void MovingState::DrawDebug(WorkerSMInfos& stateInfos)
	{
		Unit* unit = stateInfos.ControlledWorker->ManagedObject;

		Annotator::TracePath(unit->pos, m_path, *stateInfos.Datas->Actions);

		stateInfos.Datas->AddAction(std::move(Annotate::x(stateInfos.TargetPosition.x, stateInfos.TargetPosition.y)));

		stateInfos.Datas->AddAction(std::move(Annotate::text(stateInfos.TargetPosition.x + 1, stateInfos.TargetPosition.y + 1, "ST_M", 40)));
	}

	std::unique_ptr<SMState<WorkerSMInfos>> BuildingCityState::UpdateState(WorkerSMInfos& stateInfos)
	{
        Debug::Log("[SM_BuildingCityState] Update");

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

	std::unique_ptr<SMState<WorkerSMInfos>> CollectingResourcesState::UpdateState(WorkerSMInfos& stateInfos)
	{
        Debug::Log("[SM_CollectingResourcesState] Update");
		// If the city no longer needs resources
		if (stateInfos.CurrentObjective == Objective::CollectResourceForCity)
		{
			CityAI* suppliedCity = stateInfos.GetSuppliedCity();
			if (suppliedCity == nullptr || !suppliedCity->NeedResources(stateInfos.Datas->Turn))
			{
				stateInfos.AlreadyCollectedResources = true;
				return std::move(CommonActions::GoStandby(stateInfos));
			}
		}

		Unit* unit = stateInfos.ControlledWorker->ManagedObject;

		if (ShouldGoNextState(stateInfos))
		{
			return std::move(NextState(stateInfos));
		}

		return nullptr;
	}

	std::unique_ptr<SMState<WorkerSMInfos>> CollectingResourcesState::NextState(WorkerSMInfos& stateInfos)
	{
		switch (stateInfos.CurrentObjective)
		{
		case Objective::CollectResourceForSelf:
			return std::move(CommonActions::GoStandby(stateInfos));

		case Objective::CollectResourceForCity:
		{
			CityAI* suppliedCity = stateInfos.GetSuppliedCity();

			// Go standby if the supplied city has been destroyed
			if (suppliedCity == nullptr)
			{
				return std::move(CommonActions::GoStandby(stateInfos));
			}

			const CityTile* closestTile = PathFinder::GetClosestCityTile(stateInfos.ControlledWorker->ManagedObject->pos, suppliedCity->ManagedObject, *stateInfos.Datas);
			return std::unique_ptr<MovingState>(new MovingState(stateInfos, closestTile->pos));
		}

		case Objective::BuildCity:
			return std::move(CommonActions::GoBuildCity(stateInfos));
		}
		
		return nullptr;
	}

	void CollectingResourcesState::DrawDebug(WorkerSMInfos& stateInfos)
	{
		stateInfos.Datas->AddAction(std::move(Annotate::text(stateInfos.TargetPosition.x + 1, stateInfos.TargetPosition.y + 1, "ST_C", 40)));
	}

	bool CollectingResourcesState::ShouldGoNextState(WorkerSMInfos& stateInfos)
	{
		Unit* unit = stateInfos.ControlledWorker->ManagedObject;

		// Depending on the objective, the amount of resource needed to switch state changes
		switch (stateInfos.CurrentObjective)
		{
		case Objective::CollectResourceForSelf:
			return unit->getCargoSpaceLeft() < 10;

		case Objective::CollectResourceForCity:
		{
			return unit->getCargoSpaceLeft() < 10;
		}

		case Objective::BuildCity:
			return unit->getCargoSpaceLeft() == 0;
		}
		return false;
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

		stateInfos.cityTargetPosition = cityBuildTile->pos;

		Debug::Log(Utils::FormatString("[SM_State] CityCell (%i, %i)", cityBuildTile->pos.x, cityBuildTile->pos.y));

		return std::unique_ptr<MovingState>(new MovingState(stateInfos, cityBuildTile->pos, PathFindingFlags::AvoidCities));
	}

	std::unique_ptr<SMState<WorkerSMInfos>> CommonActions::GoCollectResources(WorkerSMInfos& stateInfos)
	{
		Cell* targetCell = stateInfos.Datas->GetClosestResourceCell(stateInfos.ControlledWorker->ManagedObject->pos);
		Debug::Log(Utils::FormatString("[SM_State] ResourceCell (%i, %i)", targetCell->pos.x, targetCell->pos.y));
		return std::unique_ptr<MovingState>(new MovingState(stateInfos, targetCell->pos));
	}
}