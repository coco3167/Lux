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

void UnitAI::CollectResources(CityTileAI& city)
{
}

void UnitAI::BuildCityTile()
{
}
