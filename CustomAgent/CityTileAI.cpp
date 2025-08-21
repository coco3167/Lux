#include "CityTileAI.h"

CityTileAI::CityTileAI(lux::CityTile& city) :
	CityTile(city)
{
}

bool CityTileAI::IsAvailable() const
{
	return CityTile.isAvailable;
}

// Score decrements with distance, MAX_SCORE being the score at 1 distance
int CityTileAI::UnitBuildScore(const lux::GameMap& gameMap) const
{
	int score = MAX_SCORE;
	int distance = 1;
	
	while (score > 0)
	{
		for (lux::DIRECTIONS direction : lux::ALL_DIRECTIONS)
		{
			lux::Position positionToTest = CityTile.pos.translate(direction, distance);
			if(positionToTest.x >= 0 && positionToTest.x < gameMap.width && positionToTest.y >= 0 && positionToTest.y <gameMap.height)
			{
				const lux::Cell* cellToTest = gameMap.getCellByPos(positionToTest);
				if(!cellToTest->hasResource())
				{
					break;
				}

				// TODO maybe use the type and amount of resource for the BuildScore
				return score;
			}
		}
		score--;
		distance++;
	}
	return score;
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
