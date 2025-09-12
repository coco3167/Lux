#include "CityTileAI.h"

CityTileAI::CityTileAI(lux::CityTile* tile) :
	m_tile(tile),
	m_alreadyAct(false)
{
}

bool CityTileAI::IsAvailable() const
{
	return !m_alreadyAct && m_tile->canAct() ;
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
			lux::Position positionToTest = m_tile->pos.translate(direction, distance);
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

std::string CityTileAI::BuildUnit(size_t workerCount, size_t cartCount)
{
	m_alreadyAct = true;

	size_t unitCount = workerCount + cartCount;
	size_t expectedCartCount = workerCount / WORKERS_FOREACH_CART;

	if(expectedCartCount > cartCount)
	{
		return std::move(m_tile->buildCart());
	}
	
	return std::move(m_tile->buildWorker());
}

std::string CityTileAI::Research()
{
	m_alreadyAct = true;
	return std::move(m_tile->research());
}
