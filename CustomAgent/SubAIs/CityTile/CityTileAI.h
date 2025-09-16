#pragma once
#include "lux/game_objects.hpp"

#include "SubAIs/SubAI.h"

/// <summary>
/// A wrapper arround a CityTile to which we can give orders for the current turn. This class is expected to be destroyed each turn as the city tiles do not have ID to identify them across multiple turn
/// </summary>
class CityTileAI
{
private:
    static constexpr int WORKERS_FOREACH_CART = 5;
    static constexpr int MAX_SCORE = 10;

    lux::CityTile* m_tile;

    bool m_alreadyAct;

public:
    CityTileAI(lux::CityTile* tile);

    /// <summary>
    /// Returns wether the managed city tile can make an action this turn
    /// </summary>
    bool IsAvailable() const;

    /// <summary>
    /// Returns how much the city wants to build a unit
    /// </summary>
    int UnitBuildScore(const lux::GameMap& gameMap) const;

    /// <summary>
    /// Returns the action to build a worker or a cart depending on the number of each on the game
    /// </summary>
    std::string BuildUnit(size_t workerCount, size_t cartCount);

    /// <summary>
    /// Returns the action to research
    /// </summary>
    std::string Research();
};

