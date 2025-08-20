#pragma once
#include <vector>

#include "../lux/game_objects.hpp"
#include "../lux/map.hpp"

using namespace lux;

class GameDatas
{
public:
	std::vector<string>& Actions;
	GameMap& Map;
	Player& Owner;

private:

	std::vector<Cell*> m_resourceTiles = std::vector<Cell*>();
	std::vector<float> m_cityTilesDesirability = std::vector<float>();

public:
	GameDatas(GameMap& map, std::vector<string>& actions, Player& owner);

	Cell* GetClosestResourceCell(Position startPosition) const;
	
	Cell* GetBestCityBuildingCell(Position startPosition) const;
	float GetDistanceDesirabilityFactor(Position startPosition, Position targetPosition) const;

	void Update();

private:
	void FillResourceTiles();
	void FillCityTilesDesirability();
};

