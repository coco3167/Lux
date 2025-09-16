#pragma once
#include <unordered_set>
#include <functional>

#include "SubAIs/SubAI.h"

#include "lux/game_objects.hpp"

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

    GameDatas* m_gameDatas;
    
public:
    CityAI(lux::City* city, GameDatas* gameDatas);

    /// <summary>
    /// Checks wether the city needs more resources at the current turn
    /// </summary>
    bool NeedResources(int turn);

    /// <summary>
    /// Returns the amount of resources the city needs at the given turn
    /// </summary>
    float ResourcesQuantityNeeded(int turn);

};
