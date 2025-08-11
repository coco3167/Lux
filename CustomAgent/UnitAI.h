#pragma once
#include "../lux/game_objects.hpp"

#include "CityTileAI.h"

class CityAI;

class UnitAI
{
public:
    lux::Unit& Unit;

public:
    UnitAI(lux::Unit& unit);

    // TODO checks if the unit needs more resources at the current turn
    bool NeedResources(int turn) const;

    // TODO checks if the unit can do smthing
    bool IsAvailable() const;

    // TODO Collect resources for themselves
    void CollectResources();

    // TODO Collect resources for a city
    void CollectResources(CityAI& city);

    // TODO build city at the best place
    void BuildCityTile();

    bool IsCart() const;
};

