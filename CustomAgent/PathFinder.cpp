#include "PathFinder.hpp"

#include "WorkerAI.h"
#include "CartAI.hpp"

bool PathFinder::FindPath(GameDatas& gameDatas, const Position& startPosition, const Position& targetPosition, Path& o_pathToTarget, PathFindingFlags flags)
{
    const GameMap& map = gameDatas.Map;

    o_pathToTarget.clear();

    const size_t cellsCount = static_cast<size_t>(map.height * map.width);
    std::vector<PathfinderCell> allCells{};
    allCells.reserve(cellsCount);


    for (int x = 0; x < map.width; ++x)
    {
        for (int y = 0; y < map.height; ++y)
        {
            int arrayIndex = PositionToArrayIndex(x, y, map);
            allCells[arrayIndex] = PathfinderCell{ map.getCell(x, y) };
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

            if (!CanPassThrough(currentCell, neighbouringCell, gameDatas, flags))
            {
                continue;
            }

            float gScoreAttempt = currentCell.GScore + ComputeCost(currentCell, neighbouringCell, gameDatas, flags);
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

const CityTile* PathFinder::GetClosestCityTile(const Position& position, const City* city, GameDatas gameDatas)
{
    std::vector<DIRECTIONS> path = {};
    path.reserve(10);

    size_t shortestPathLengh = 999999;
    const CityTile* closestCityTile = nullptr;

    for (const CityTile& tile : city->citytiles)
    {
        PathFinder::FindPath(gameDatas, position, tile.pos, path);
        if (path.size() < shortestPathLengh)
        {
            shortestPathLengh = path.size();
            closestCityTile = &tile;
        }
        path.clear();
    }

    return closestCityTile;
}

float PathFinder::ComputeHeuristic(const lux::Position& cellPosition, const lux::Position& targetPosition)
{
    return cellPosition.distanceTo(targetPosition);
}

bool PathFinder::CanPassThrough(const PathfinderCell& currentCell, const PathfinderCell& neighbouringCell, GameDatas& gameDatas, PathFindingFlags flags)
{
    CityTile* neighbouringCityTile = neighbouringCell.Cell->citytile;
    if (neighbouringCityTile != nullptr)
    {
        if (Utils::HasFlag(flags, PathFindingFlags::AvoidCities) || neighbouringCityTile->team != gameDatas.Owner->team)
        {
            Debug::LogWarning(Utils::FormatString("Cant pass through city et (%i, %i)", neighbouringCell.Cell->pos.x, neighbouringCell.Cell->pos.y));
            return false; // Can't pass through a opponent's city tile
        }
    }
    return true;
}

float PathFinder::ComputeCost(const PathfinderCell& currentCell, const PathfinderCell& neighbouringCell, GameDatas& gameDatas, PathFindingFlags flags)
{
    CityTile* neighbouringCityTile = neighbouringCell.Cell->citytile;

    if (!Utils::HasFlag(flags, PathFindingFlags::IgnoreUnits))
    {
        bool allyUnitInNeighbouringCell = !gameDatas.PositionAvailableNextTurn(neighbouringCell.Cell->pos);
        
        if (allyUnitInNeighbouringCell && neighbouringCityTile == nullptr)
        {
            return WALL_COST; // Can't pass through an ally outside a city
        }
    }

    if (neighbouringCell.Cell->road > 0.0f)
    {
        return 1.0f / neighbouringCell.Cell->road;
    }

    return 1.0f;
}

int PathFinder::PositionToArrayIndex(const lux::Position& position, const lux::GameMap& map)
{
    return PositionToArrayIndex(position.x, position.y, map);
}

int PathFinder::PositionToArrayIndex(int x, int y, const lux::GameMap& map)
{
    return x + y * map.width;
}

void PathFinder::ReconstructPath(PathfinderCell& targetCell, Path& o_pathToTarget)
{
    PathfinderCell& cell = targetCell;
    while (cell.ComeFromDirection != DIRECTIONS::CENTER)
    {
        o_pathToTarget.push_back(Utils::GetOppositeDirection(cell.ComeFromDirection));
        cell = *cell.ComeFromCell;
    }
    std::reverse(o_pathToTarget.begin(), o_pathToTarget.end());
}
