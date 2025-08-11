#pragma once
#include "../lux/game_objects.hpp"

class CityAI
{
public:
    lux::City& City;

    // TODO checks if the city needs more resources at the current turn
    bool NeedResources(int turn) const;

    // TODO gets how much resources the unit needs
    int ResourcesQuantityNeeded(int turn) const;
};
