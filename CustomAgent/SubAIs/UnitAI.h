#pragma once
#include "SubAIs/SubAI.h"
#include "lux/game_objects.hpp"

/// <summary>
/// Base class for wrappers arround Unit that allows them to move
/// </summary>
class UnitAI : public SubAI<lux::Unit>
{
public:
    Position PositionNextTurn;

    UnitAI(lux::Unit* unit);

    virtual void BeginTurn();

    /// <summary>
    /// Moves the unit in the given direction if possible. Otherwise turns the direction anticlockwise until a valid direction is found to avoid getting stuck
    /// </summary>
    virtual void MoveUnit(GameDatas* gameDatas, DIRECTIONS direction);
};

