#pragma once

#include <limits>
#include <queue>
#include <vector>
#include <array>
#include <algorithm>

#include "../lux/map.hpp"
#include "../lux/constants.hpp"
#include "../lux/game_objects.hpp"

#include "Utils.hpp"
#include "CustomPriorityQueue.hpp"


using namespace lux;

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
public:
    static bool FindPath(const GameMap& map, const Position& startPosition, const Position& targetPosition, const Player& curentPlayer, std::vector<DIRECTIONS>& o_pathToTarget)
    {
        const size_t cellsCount = static_cast<size_t>(map.height * map.width);
        std::vector<PathfinderCell> allCells{};
        allCells.reserve(cellsCount);


        for (int x = 0; x < map.width; ++x)
        {    
            for (int y = 0; y < map.height; ++y)
            {
                int arrayIndex = PositionToArrayIndex(x, y, map);
                allCells[arrayIndex] = PathfinderCell{map.getCell(x, y)};
            }
        }

        CustomPriorityQueue<PathfinderCell*, std::vector<PathfinderCell*>, PathfinderCellComparer> openQueue = {};

        int cellIndex = 0;

        PathfinderCell* startCell = &allCells[PositionToArrayIndex(startPosition, map)];
        startCell->GScore = 0;
        startCell->FScore = ComputeHeuristic(startPosition, targetPosition);
        openQueue.push(startCell);

        while (!openQueue.empty())
        {

            PathfinderCell& currentCell = *openQueue.top();
            openQueue.pop();

            cellIndex++;

            if (currentCell.Cell->pos == targetPosition)
            {
                ReconstructPath(currentCell, o_pathToTarget);
                return true;
            }

            for (DIRECTIONS dir : ALL_DIRECTIONS)
            {
                lux::Position neighbouringPosition = currentCell.Cell->pos.translate(dir, 1);
                if (!Utils::IsInMap(neighbouringPosition, map))
                {
                    continue;
                }

                PathfinderCell& neighbouringCell = allCells[PositionToArrayIndex(neighbouringPosition, map)];
                
                float gScoreAttempt = currentCell.GScore + ComputeCost(currentCell, neighbouringCell, curentPlayer);
                if (gScoreAttempt >= neighbouringCell.GScore)
                {
                    continue;
                }

                
                neighbouringCell.ComeFromDirection = Utils::GetOppositeDirection(dir);
                neighbouringCell.ComeFromCell = &currentCell;
                neighbouringCell.GScore = gScoreAttempt;
                neighbouringCell.FScore = gScoreAttempt + ComputeHeuristic(neighbouringPosition, targetPosition);

                if (!openQueue.Contains(&neighbouringCell))
                {
                    openQueue.push(&neighbouringCell);
                }
            }
        }

        return false;
    }

    static const CityTile* GetClosestCityTile(const Position& position, const City* city, GameMap& map, Player& player) 
    {
        std::vector<DIRECTIONS> path = {};
        path.reserve(10);

        size_t shortestPathLengh = 999999;
        const CityTile* closestCityTile = nullptr;

        for (const CityTile& tile : city->citytiles) 
        {
            PathFinder::FindPath(map, position, tile.pos, player, path);
            if (path.size() < shortestPathLengh) 
            {
                shortestPathLengh = path.size();
                closestCityTile = &tile;
            }
            path.clear();
        }

        return closestCityTile;
    }

private:
    static float ComputeHeuristic(const lux::Position& cellPosition, const lux::Position& targetPosition)
    {
        return cellPosition.distanceTo(targetPosition);
    }

    static float ComputeCost(const PathfinderCell& currentCell, const PathfinderCell& neighbouringCell, const Player& currentPlayer)
    {
        CityTile* neighbouringCityTile = neighbouringCell.Cell->citytile;
        if (neighbouringCityTile != nullptr && neighbouringCityTile->team != currentPlayer.team)
        {
            return 99999999.0f; // Can't pass through a opponent's city tile
        }

        bool allyUnitInNeighbouringCell = false;
        for (const Unit& allyUnit : currentPlayer.units) 
        {
            if (allyUnit.pos == neighbouringCell.Cell->pos) 
            {
                allyUnitInNeighbouringCell = true;
                break;
            }
        }

        if (allyUnitInNeighbouringCell && neighbouringCityTile == nullptr)
        {
            return 99999999.0f; // Can't pass through an ally outside a city
        }

        if (neighbouringCell.Cell->road > 0.0f) 
        {
            return 1.0f / neighbouringCell.Cell->road;
        }

        return 1.0f;
    }

    static int PositionToArrayIndex(const lux::Position& position, const lux::GameMap& map)
    {
        return PositionToArrayIndex(position.x, position.y, map);
    }
    
    static int PositionToArrayIndex(int x, int y, const lux::GameMap& map)
    {
        return x + y * map.width;
    }

    static void ReconstructPath(PathfinderCell& targetCell, std::vector<DIRECTIONS>& o_pathToTarget)
    {
        PathfinderCell& cell = targetCell;
        while (cell.ComeFromDirection != DIRECTIONS::CENTER)
        {
            o_pathToTarget.push_back(Utils::GetOppositeDirection(cell.ComeFromDirection));
            cell = *cell.ComeFromCell;
        }
        //std::reverse(o_pathToTarget.begin(), o_pathToTarget.end());
    }
};