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
	Position targetPosition = ManagedObject->pos.translate(direction, 1);
	int directionCount = 0;

	while (!gameDatas->PositionAvailableNextTurn(targetPosition) && directionCount < 4)
	{
		direction = Utils::TurnDirection(direction, false);
		targetPosition = ManagedObject->pos.translate(direction, 1);

		++directionCount;
	}

	if (directionCount == 3)
	{
		Debug::LogWarning("[UnitAI] No available direction");
		return;
	}

	PositionNextTurn = targetPosition;
	gameDatas->AddAction(std::move(ManagedObject->move(direction)));
}


