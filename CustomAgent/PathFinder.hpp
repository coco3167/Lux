#pragma once

#include <limits>
#include <queue>
#include <array>
#include <algorithm>

#include "../lux/map.hpp"
#include "../lux/constants.hpp"
#include "../lux/game_objects.hpp"

#include "Debug.h"
#include "Utils.hpp"
#include "Alias.h"
#include "GameDatas.h"
#include "CustomPriorityQueue.hpp"


using namespace lux;

enum PathFindingFlags : int
{
    None = 0,
    AvoidCities = 1 << 0,
    IgnoreUnits = 1 << 1,
};

class PathfinderCell
{
public:
    float GScore;
    float FScore;
    const lux::Cell* Cell;

    DIRECTIONS ComeFromDirection;
    PathfinderCell* ComeFromCell;

    public:
    PathfinderCell(const lux::Cell* cell) : 
        Cell(cell),
        GScore(std::numeric_limits<float>::max()),
        FScore(std::numeric_limits<float>::max()),
        ComeFromDirection(DIRECTIONS::CENTER),
        ComeFromCell(nullptr)
    {
        
    }
    
};

class PathfinderCellComparer
{
public:
    constexpr bool operator()(const PathfinderCell* lhs, const PathfinderCell* rhs) const 
    {
        return lhs->FScore > rhs->FScore;
    }
};

class PathFinder
{
private:
    static constexpr float WALL_COST = 999.0f;
public:
    static bool FindPath(
        GameDatas& gameDatas,
        const Position& startPosition,
        const Position& targetPosition,
        Path& o_pathToTarget,
        PathFindingFlags flags = PathFindingFlags::None);


    static const CityTile* GetClosestCityTile(const Position& position, const City* city, GameDatas gameDatas);

private:
    static float ComputeHeuristic(const lux::Position& cellPosition, const lux::Position& targetPosition);


    static bool CanPassThrough(const PathfinderCell& currentCell, const PathfinderCell& neighbouringCell, GameDatas& gameDatas, PathFindingFlags flags);

    static float ComputeCost(const PathfinderCell& currentCell, const PathfinderCell& neighbouringCell, const Player* currentPlayer, PathFindingFlags flags);

    static int PositionToArrayIndex(const lux::Position& position, const lux::GameMap& map);

    
    static int PositionToArrayIndex(int x, int y, const lux::GameMap& map);


    static void ReconstructPath(PathfinderCell& targetCell, Path& o_pathToTarget);

};