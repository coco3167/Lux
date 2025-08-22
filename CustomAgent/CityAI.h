#pragma once
#include <unordered_set>
#include <functional>

#include "SubAI.h"

#include "../lux/game_objects.hpp"

struct PositionHash
{
	size_t operator()(const lux::Position& position) const
    {
        size_t xHash = std::hash<int>{}(position.x);
        size_t yHash = std::hash<int>{}(position.y);
        return xHash ^ (yHash << 1); // or use boost::hash_combine
    }
};


class CityAI : public SubAI<lux::City>
{
private:
    std::unordered_set<lux::Position, PositionHash> m_unorderedCityTilePositions;
    
public:
    CityAI(lux::City* city);

    // TODO checks if the city needs more resources at the current turn
    bool NeedResources(int turn);

    // TODO checks if has available cityTiles
    bool IsAvailable() const;

    // TODO gets how much resources the unit needs
    float ResourcesQuantityNeeded(int turn);

    //TODO research
    void Research();

};
