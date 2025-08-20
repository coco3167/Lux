#pragma once
#include "SubAI.h"

#include "../lux/game_objects.hpp"

class CityAI : public SubAI<lux::City>
{

public:
    CityAI(lux::City& city);

    // TODO checks if the city needs more resources at the current turn
    bool NeedResources(int turn) const;

    // TODO checks if has available cityTiles
    bool IsAvailable() const;

    // TODO gets how much resources the unit needs
    int ResourcesQuantityNeeded(int turn) const;

    // TODO returns how much the city wants to build a unit
    int UnitBuildScore() const;

    // TODO build a unit
    void BuildUnit();

    //TODO research
    void Research();
};

