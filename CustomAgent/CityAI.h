#pragma once
#include "../lux/game_objects.hpp"

class CityAI
{
private:
    std::unordered_map<lux::Position, lux::CityTile> m_unorderedCityTilePositions;
    
public:
    lux::City& City;
    
    bool NeedResources(int turn);
    float ResourcesQuantityNeeded(int turn);
};
