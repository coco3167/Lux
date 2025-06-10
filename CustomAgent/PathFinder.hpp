#include <limits>
#include <queue>
#include <vector>
#include <array>
#include <algorithm>

#include "../lux/map.hpp"
#include "../lux/constants.hpp"

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
        return lhs->FScore < rhs->FScore;
    }
};

class PathFinder
{
public:
    static bool FindPath(const GameMap& map, Position& startPosition, Position& targetPosition, std::vector<DIRECTIONS>& o_pathToTarget, std::vector<string>& actions)
    {
        int cellsCount = map.height * map.width;
        std::vector<PathfinderCell> allCells = {};
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

        openQueue.push(&allCells[PositionToArrayIndex(startPosition, map)]);

        while (!openQueue.empty())
        {

            PathfinderCell& currentCell = *openQueue.top();
            openQueue.pop();

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
                
                float gScoreAttempt = currentCell.GScore + ComputeCost(currentCell, neighbouringCell);
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

private:
    static float ComputeHeuristic(lux::Position& cellPosition, lux::Position& targetPosition)
    {
        return cellPosition.distanceTo(targetPosition);
    }

    static float ComputeCost(const PathfinderCell& currentCell, const PathfinderCell& neighbouringCell)
    {
        return 1.0f;
    }

    static int PositionToArrayIndex(lux::Position& position, const lux::GameMap& map)
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