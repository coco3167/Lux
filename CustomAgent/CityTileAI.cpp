#include "CityTileAI.h"

CityTileAI::CityTileAI(lux::CityTile& city) :
	CityTile(city)
{
}

bool CityTileAI::IsAvailable() const
{
	return CityTile.isAvailable;
}

int CityTileAI::UnitBuildScore() const
{
	return 0;
}

std::string CityTileAI::BuildUnit(const std::vector<UnitAI>& units) const
{
	int cartNb = 0;
	for (const UnitAI& unit : units)
	{
		cartNb += unit.IsCart();
	}

	if(cartNb/static_cast<float>(units.size()) < CART_PERCENTAGE)
	{
		return CityTile.buildCart();
	}
	
	return CityTile.buildWorker();
}

std::string CityTileAI::Research() const
{
	return CityTile.research();
}
