#pragma once
#include "../lux/game_objects.hpp"

#include "SubAI.h"

class CityTileAI
{
private:
    static constexpr float CART_PERCENTAGE = 0.1f;
    static constexpr int MAX_SCORE = 10;

    lux::CityTile* m_tile;

public:
    CityTileAI(lux::CityTile* tile);
    bool IsAvailable() const;

    // TODO returns how much the city wants to build a unit
    int UnitBuildScore(const lux::GameMap& gameMap) const;

    std::string BuildUnit(size_t workerCount, size_t cartCount) const;
    std::string Research() const;
};

