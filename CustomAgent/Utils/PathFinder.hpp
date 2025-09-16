#pragma once

#include <limits>
#include <queue>
#include <array>
#include <algorithm>

#include "lux/map.hpp"
#include "lux/constants.hpp"
#include "lux/game_objects.hpp"

#include "Utils/Debug/Debug.h"
#include "Utils/Utils.hpp"
#include "Utils/Alias.h"
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
    /// <summary>
    /// Computes the shortest path from <paramref name="startPosition"/> to <paramref name="targetPosition"/> and stores it in <paramref name="o_pathToTarget"/>
    /// </summary>
    static bool FindPath(
        GameDatas& gameDatas,
        const Position& startPosition,
        const Position& targetPosition,
        Path& o_pathToTarget,
        PathFindingFlags flags = PathFindingFlags::None);

    /// <summary>
    /// Returns the closest CityTile of the given City from the given position
    /// </summary>
    static const CityTile* GetClosestCityTile(const Position& position, const City* city, GameDatas& gameDatas);

private:
    static float ComputeHeuristic(const lux::Position& cellPosition, const lux::Position& targetPosition);


    static bool CanPassThrough(const PathfinderCell& currentCell, const PathfinderCell& neighbouringCell, GameDatas& gameDatas, PathFindingFlags flags);

    static float ComputeCost(const PathfinderCell& currentCell, const PathfinderCell& neighbouringCell, GameDatas& gameDatas, PathFindingFlags flags);

    static int PositionToArrayIndex(const lux::Position& position, const lux::GameMap& map);

    
    static int PositionToArrayIndex(int x, int y, const lux::GameMap& map);


    static void ReconstructPath(PathfinderCell& targetCell, Path& o_pathToTarget);

};