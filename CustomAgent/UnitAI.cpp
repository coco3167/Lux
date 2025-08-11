#include "UnitAI.h"

UnitAI::UnitAI(lux::Unit& unit) : 
	Unit(unit)
{
}

bool UnitAI::NeedResources(int turn) const
{
	return false;
}

bool UnitAI::IsAvailable() const
{
	return false;
}

void UnitAI::CollectResources()
{
}

void UnitAI::CollectResources(CityAI& city)
{
}

void UnitAI::BuildCityTile()
{
}

bool UnitAI::IsCart() const
{
	return Unit.isCart();
}
