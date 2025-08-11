#pragma once
#include "UnitAI.h"
#include "../lux/game_objects.hpp"

class CityTileAI
{
private:
    constexpr float CART_PERCENTAGE = .1f;
    
public:
    lux::CityTile& CityTile;

public:
    CityTileAI(lux::CityTile& city);
    bool IsAvailable() const;

    // TODO returns how much the city wants to build a unit
    int UnitBuildScore() const;

    std::string BuildUnit(const std::vector<UnitAI>& units) const;
    std::string Research() const;
};

