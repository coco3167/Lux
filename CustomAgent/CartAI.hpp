#pragma once
#include <string>
#include <vector>
#include "../lux/game_objects.hpp"
#include "../lux/position.hpp"

#include "CityAI.h"
#include "UnitAI.h"

class CartAI
{
enum CartState
{
    STANDBY,
    BUILDING_ROAD,
    RESUPPLYING_UNIT,
    RESUPPLYING_CITY
};

public:
    static const int FUEL_NEEDED_FOR_THE_NIGHT = 100;

    lux::Unit& cart;
    lux::GameMap& map;

    CartState state;
    lux::Position destination;
    lux::Position origin;
    lux::Unit* resupplyTarget;
    

public:
    CartAI(lux::Unit& unit, lux::GameMap& map);

    void Update();

    // TODO checks if the unit needs more resources at the current turn
    bool NeedResources(int turn) const;

    // TODO checks if the unit can do something
    bool IsAvailable() const;
    
    bool DestinationReached();

    void UpdateDestination();

    void Resupply(lux::Unit& unit);
    void Resupply(lux::CityTile& city);

    // TODO start to build a road from start to end
    void BuildRoad(lux::Position start, lux::Position end);

    std::vector<std::string> Transfer(lux::Unit unit);
};


