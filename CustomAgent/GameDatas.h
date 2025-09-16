#pragma once
#include <vector>

#include "lux/game_objects.hpp"
#include "lux/map.hpp"

#include "SubAIs/Worker/WorkerObjective.hpp"

using namespace lux;

class WorkerAI;
class CartAI;
class CityAI;

/// <summary>
/// Class used to store all the datas of the game and to pass them to all the SubAIs
/// </summary>
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

	/// <summary>
	/// Updates the stored state of the game using the newest datas
	/// </summary>
	void Update(std::vector<string>* actions, Player* owner, int turn);

	/// <summary>
	/// Insert an action for the turn
	/// </summary>
	void AddAction(string&& action);

	/// <summary>
	/// Sets the desirability of the given tile position to 0 in order to prevent workers to build cities on it
	/// </summary>
	void NullifyCityTileDesirability(const Position& position);

	/// <summary>
	/// Returns the closest resource cell from the given position
	/// </summary>
	Cell* GetClosestResourceCell(Position startPosition) const;
	
	/// <summary>
	/// Returns the best tile to build a city on. The desirability is based on the resources nearby, the other city tiles and the distance from the given start position
	/// </summary>
	Cell* GetBestCityBuildingCell(Position startPosition) const;
	float GetDistanceDesirabilityFactor(Position startPosition, Position targetPosition) const;

	/// <summary>
	/// Returns the closest worker from the given position that has the given objective
	/// </summary>
	WorkerAI* GetClosestWorker(Position startPosition, WorkerSM::Objective desiredObjective) const;

	int TurnsUntilNight() const;
	int TurnsUntilDay() const;

	bool IsNight() const;

	/// <summary>
	/// Returns the position of all the ally units on the next turn. Depending on when this function is called during the day, the return value may vary because the other units may have move
	/// </summary>
	std::vector<Position> GetNextTurnPositions() const;
	/// <summary>
	/// Returns wether an ally unit will be on the given position on the next turn. Depending on when this function is called during the day, the return value may vary because the other units may have move
	/// </summary>
	bool PositionAvailableNextTurn(Position position) const;


private:
	void FillResourceTiles();
	void FillCityTilesDesirability();
	float GetTileBaseCityDesirability(Cell* cell) const;
	void ApplyResourceDesirability();
	void ApplyCityProximityDesirability();

	int GetTimeOfDay() const;
};

