#pragma once
#include "SubAI.h"
#include "../lux/game_objects.hpp"

class UnitAI : public SubAI<lux::Unit>
{
public:
    Position PositionNextTurn;

    UnitAI(lux::Unit* unit);

    void BeginTurn();
};

