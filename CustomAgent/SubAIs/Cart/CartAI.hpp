#pragma once
#include <string>
#include <vector>

#include "lux/game_objects.hpp"
#include "lux/position.hpp"

#include "SubAIs/City/CityAI.h"
#include "SubAIs/Worker/WorkerAI.h"

#include "SubAIs/UnitAI.h"
#include "GameDatas.h"

/// <summary>
/// A wrapper arround a Cart to which we can give orders it will follow for multiple turns
/// </summary>
class CartAI : public UnitAI
{
enum CartState
{
    STANDBY,
    BUILDING_ROAD,
    REQUEST_FROM_UNIT,
    RESUPPLYING_CITY
};

public:
    static const int FUEL_NEEDED_FOR_THE_NIGHT = 100;

    GameDatas* m_gameDatas;

    CartState state;
    lux::Position destination;
    lux::Position origin;
    WorkerAI* requestTarget;
    CityAI* resupplyTarget;

    bool m_hasDestination;
    

public:
    CartAI(lux::Unit* unit, GameDatas* gameDatas);

    void Update();

    /// <summary>
    /// Checks wether the cart needs more resources at the current turn
    /// </summary>
    bool NeedResources(int turn) const;

    /// <summary>
    /// Returns wether the managed cart can make an action this turn
    /// </summary>
    bool IsAvailable() const;

    /// <summary>
    /// Instruct the cart to go get resources from the closest worker that collects resources for a city and to bring them to the given city
    /// </summary>
    bool TryGoResupply(CityAI* city);

    /// <summary>
    /// Instruct the cart to make back and forth between the two given positions to build a road
    /// </summary>
    void BuildRoad(lux::Position start, lux::Position end);

    void Transfer(lux::Unit& unit);

private:
    bool DestinationReached() const;

    void UpdateDestination();

    void Move();
    void GoRequestFromUnit(WorkerAI* unit);

    void GoResupplyClosestCityTile(CityAI* city);
};


