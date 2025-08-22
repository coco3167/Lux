#pragma once
#include <vector>

#include "../lux/game_objects.hpp"
#include "../lux/map.hpp"

using namespace lux;

class GameDatas
{
public:
	GameMap& Map;
	Player& Owner;
	std::vector<string>* Actions;

private:

	std::vector<Cell*> m_resourceTiles = std::vector<Cell*>();
	std::vector<float> m_cityTilesDesirability = std::vector<float>();

public:
	GameDatas(GameMap& map, Player& owner);

	void Update(std::vector<string>* actions);
	void AddAction(string&& action);

	Cell* GetClosestResourceCell(Position startPosition) const;
	
	Cell* GetBestCityBuildingCell(Position startPosition) const;
	float GetDistanceDesirabilityFactor(Position startPosition, Position targetPosition) const;


private:
	void FillResourceTiles();
	void FillCityTilesDesirability();
};

