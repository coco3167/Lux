#include "CityTileAI.h"

CityTileAI::CityTileAI(lux::CityTile& city) :
	CityTile(city)
{
}

bool CityTileAI::IsAvailable() const
{
	return false;
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
