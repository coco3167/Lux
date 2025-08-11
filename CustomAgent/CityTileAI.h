#pragma once
#include "../lux/game_objects.hpp"

class CityTileAI
{
public:
    lux::CityTile& CityTile;

public:
    CityTileAI(lux::CityTile& city);

    // TODO checks if has available cityTiles
    bool IsAvailable() const;

    // TODO returns how much the city wants to build a unit
    int UnitBuildScore() const;

    // TODO build a unit
    void BuildUnit();

    //TODO research
    void Research();
};

