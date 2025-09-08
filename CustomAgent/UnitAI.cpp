#include "UnitAI.h"

UnitAI::UnitAI(lux::Unit* unit) :
	SubAI(unit, unit->id)
{
}

void UnitAI::BeginTurn()
{
	PositionNextTurn = ManagedObject->pos;
}
