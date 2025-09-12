#include "UnitAI.h"

#include "Utils.hpp"
#include "Debug.h"

UnitAI::UnitAI(lux::Unit* unit) :
	SubAI(unit, unit->id)
{
}

void UnitAI::BeginTurn()
{
	PositionNextTurn = ManagedObject->pos;
}

void UnitAI::MoveUnit(GameDatas* gameDatas, DIRECTIONS direction)
{
	Debug::LogWarning("[UnitAI][MoveUnit] Init");
	Position targetPosition = ManagedObject->pos.translate(direction, 1);
	int directionCount = 0;

	Debug::LogWarning("[UnitAI][MoveUnit] CheckAvailability");
	while (!gameDatas->PositionAvailableNextTurn(targetPosition) && directionCount < 4)
	{
		Debug::LogWarning("[UnitAI][MoveUnit] Turn direction");
		direction = Utils::TurnDirection(direction, false);
		targetPosition = ManagedObject->pos.translate(direction, 1);

		++directionCount;
	}

	if (directionCount == 3)
	{
		Debug::LogWarning("[UnitAI][MoveUnit] No available direction");
		return;
	}

	Debug::LogWarning(Utils::FormatString("[UnitAI][MoveUnit] Move to (%i; %i)", targetPosition.x, targetPosition.y));
	PositionNextTurn = targetPosition;
	gameDatas->AddAction(std::move(ManagedObject->move(direction)));
}


