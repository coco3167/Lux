#pragma once
#include <vector>

#include "../lux/game_objects.hpp"
#include "../lux/map.hpp"

#include "WorkerObjective.hpp"

using namespace lux;

class WorkerAI;
class CartAI;
class CityAI;

class GameDatas
{
public:
	GameMap& Map;
	Player* Owner;
	std::vector<string>* Actions;
	int Turn;

	std::vector<std::unique_ptr<WorkerAI>>* WorkerAIs;
	std::vector<std::unique_ptr<CartAI>>* CartAIs;
	std::vector<std::unique_ptr<CityAI>>* CityAIs;

private:

	std::vector<Cell*> m_resourceTiles = std::vector<Cell*>();
	std::vector<float> m_cityTilesDesirability = std::vector<float>();


public:
	GameDatas(GameMap& map, Player* owner);

	void Update(std::vector<string>* actions, Player* owner, int turn);
	void AddAction(string&& action);

	void NullifyCityTileDesirability(const Position& position);

	Cell* GetClosestResourceCell(Position startPosition) const;
	
	Cell* GetBestCityBuildingCell(Position startPosition) const;
	float GetDistanceDesirabilityFactor(Position startPosition, Position targetPosition) const;

	WorkerAI* GetClosestWorker(Position startPosition, WorkerSM::Objective desiredObjective) const;

	int TurnsUntilNight() const;
	int TurnsUntilDay() const;

	bool IsNight() const;

	std::vector<Position> GetNextTurnPositions() const;
	bool PositionAvailableNextTurn(Position position) const;


private:
	void FillResourceTiles();
	void FillCityTilesDesirability();
	void ApplyResourceDesirability();
	void ApplyCityProximityDesirability();

	int GetTimeOfDay() const;
};

