#pragma once
#include "SubAIs/SubAI.h"
#include "lux/game_objects.hpp"

class UnitAI : public SubAI<lux::Unit>
{
public:
    Position PositionNextTurn;

    UnitAI(lux::Unit* unit);

    virtual void BeginTurn();

    virtual void MoveUnit(GameDatas* gameDatas, DIRECTIONS direction);
};

