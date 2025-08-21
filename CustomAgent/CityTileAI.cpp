#include "CityTileAI.h"

CityTileAI::CityTileAI(lux::CityTile* tile) :
	SubAI(tile)
{
}

bool CityTileAI::IsAvailable() const
{
	return ManagedObject->isAvailable;
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
			lux::Position positionToTest = ManagedObject->pos.translate(direction, distance);
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

std::string CityTileAI::BuildUnit(size_t workerCount, size_t cartCount) const
{
	size_t unitCount = workerCount + cartCount;

	if(cartCount / static_cast<float>(unitCount) < CART_PERCENTAGE)
	{
		return std::move(ManagedObject->buildCart());
	}
	
	return std::move(ManagedObject->buildWorker());
}

std::string CityTileAI::Research() const
{
	return std::move(ManagedObject->research());
}
