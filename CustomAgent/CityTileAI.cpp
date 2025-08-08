#include "CityTileAI.h"

CityTileAI::CityTileAI(lux::CityTile& city) :
	CityTile(city)
{
}

bool CityTileAI::NeedResources(int turn) const
{
	return false;
}

bool CityTileAI::IsAvailable() const
{
	return false;
}

int CityTileAI::ResourcesQuantityNeeded(int turn) const
{
	return 0;
}

int CityTileAI::UnitBuildScore() const
{
	return 0;
}

void CityTileAI::BuildUnit()
{
}

void CityTileAI::Research()
{
}
