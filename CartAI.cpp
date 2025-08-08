#include "CartAI.hpp"

CartAI::CartAI(lux::Unit& unit) :
	Unit(unit)
{
}

bool CartAI::NeedResources(int turn) const
{
	return false;
}

bool CartAI::IsAvailable() const
{
	return false;
}

void CartAI::CollectResources()
{
}

void CartAI::CollectResources(CityAI& city)
{
}

void CartAI::BuildCityTile()
{
}