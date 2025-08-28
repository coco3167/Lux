#include "GameDatas.h"

#include <algorithm>

#include "PathFinder.hpp"

#include "WorkerAI.h"

#include "Utils.hpp"
#include "Debug.h"

GameDatas::GameDatas(GameMap& map, Player* owner) :
    Map(map),
    Actions(nullptr),
    Owner(owner),
    Turn(0),
    m_cityTilesDesirability(map.height * map.width)
{
}

void GameDatas::Update(std::vector<string>* actions, Player* owner, int turn)
{
    Actions = actions;
    Turn = turn;
    FillResourceTiles();
    FillCityTilesDesirability();
}

void GameDatas::AddAction(string&& action)
{
    if (!Actions) 
    {
        return;
    }

    Actions->push_back(std::move(action));
}

Cell* GameDatas::GetClosestResourceCell(Position startPosition) const
{
    Cell* closestResourceTile = nullptr;
    float closestDist = 9999999.0f;
    for (auto it = m_resourceTiles.begin(); it != m_resourceTiles.end(); it++)
    {
        auto cell = *it;
        if (cell->resource.type == ResourceType::coal && !Owner->researchedCoal()) continue;
        if (cell->resource.type == ResourceType::uranium && !Owner->researchedUranium()) continue;
        float dist = cell->pos.distanceTo(startPosition);
        if (dist < closestDist)
        {
            closestDist = dist;
            closestResourceTile = cell;
        }
    }
    return closestResourceTile;
}

Cell* GameDatas::GetBestCityBuildingCell(Position startPosition) const
{
    Cell* mostDesirableCityTile = nullptr;
    float mostDesirableCityTileScore = 0.0f;

    float tileDesirability;
    for (int x = 0; x < Map.width; ++x)
    {
        for (int y = 0; y < Map.height; ++y)
        {
            float factor = GetDistanceDesirabilityFactor(startPosition, {x, y});
            tileDesirability = m_cityTilesDesirability[y * Map.width + x] * factor;
            if (tileDesirability > mostDesirableCityTileScore)
            {
                mostDesirableCityTileScore = tileDesirability;
                mostDesirableCityTile = Map.getCell(x, y);
            }
        }
    }

    return mostDesirableCityTile;

}

float GameDatas::GetDistanceDesirabilityFactor(Position startPosition, Position targetPosition) const
{
    // std::vector<DIRECTIONS> path = {};
    // path.reserve(10);

    // PathFinder::FindPath(Map, startPosition, targetPosition, Owner, path);
    int pathLength = startPosition.distanceTo(targetPosition);

    return static_cast<float>(Utils::Clamp(1.0 - std::log10(pathLength) / 2.0, 0.0001, 2.0));
}

WorkerAI* GameDatas::GetClosestWorker(Position startPosition, WorkerSM::Objective desiredObjective) const
{
    WorkerAI* closestWorker = nullptr;
    float closestDist = 9999999.0f;
    for (auto it = WorkerAIs->begin(); it != WorkerAIs->end(); it++)
    {
        WorkerAI* worker = &*it;

        if (worker->GetCurrentObjective() != desiredObjective)
        {
            continue;
        }

        float dist = worker->ManagedObject->pos.distanceTo(startPosition);
        if (dist < closestDist)
        {
            closestDist = dist;
            closestWorker = worker;
        }
    }
    return closestWorker;
}

int GameDatas::TurnsUntilNight() const
{
    if (IsNight())
    {
        return 0;
    }
    return 30 - GetTimeOfDay();
}

int GameDatas::TurnsUntilDay() const
{
    if (!IsNight())
    {
        return 0;
    }
    return 40 - GetTimeOfDay();
}

bool GameDatas::IsNight() const
{
    return GetTimeOfDay() > 29;
}

void GameDatas::FillResourceTiles()
{
    m_resourceTiles.clear();

    for (int y = 0; y < Map.height; y++)
    {
        for (int x = 0; x < Map.width; x++)
        {
            Cell* cell = Map.getCell(x, y);
            if (cell->hasResource())
            {
                m_resourceTiles.push_back(cell);
            }
        }
    }
}

void GameDatas::FillCityTilesDesirability()
{
    for (int x = 0; x < Map.width; ++x)
    {
        for (int y = 0; y < Map.height; ++y)
        {
            m_cityTilesDesirability[y * Map.width + x] = 100.0f;
        }
    }

    ApplyResourceDesirability();
    ApplyCityProximityDesirability();
}

void GameDatas::ApplyResourceDesirability()
{
    const int weightRange = 2;
    for (const Cell* resourceTile : m_resourceTiles)
    {
        m_cityTilesDesirability[resourceTile->pos.y * Map.width + resourceTile->pos.x] = 0.0f;

        float resourceDesirability;
        switch (resourceTile->resource.type)
        {
        case ResourceType::wood:
            resourceDesirability = 1.1f;

        case ResourceType::coal:
            if (!Owner->researchedCoal())
            {
                continue;
            }
            resourceDesirability = 1.3f;

        case ResourceType::uranium:
            if (!Owner->researchedUranium())
            {
                continue;
            }
            resourceDesirability = 1.8f;
        }


        for (int dx = -weightRange; dx <= weightRange; ++dx)
        {
            const int yRange = weightRange - std::abs(dx);
            for (int dy = -yRange; dy <= yRange; ++dy)
            {
                const int weightX = resourceTile->pos.x + dx;
                const int weightY = resourceTile->pos.y + dy;

                if (!Utils::IsInMap(weightX, weightY, Map))
                {
                    continue;
                }

                m_cityTilesDesirability[weightY * Map.width + weightX] *= resourceDesirability;
            }
        }
    }
}

void GameDatas::ApplyCityProximityDesirability()
{
    const int weightRange = 5;
    std::vector<Position> tilesPositions{};
    tilesPositions.reserve(Owner->cities.size() * 5);

    for (std::map<string, City>::iterator it = Owner->cities.begin(); it != Owner->cities.end(); it++)
    {
        City* city = &it->second;
        for (CityTile& tile : city->citytiles)
        {
            tilesPositions.push_back(tile.pos);

            for (int dx = -weightRange; dx <= weightRange; ++dx)
            {
                const int yRange = weightRange - std::abs(dx);
                for (int dy = -yRange; dy <= yRange; ++dy)
                {
                    const int weightX = tile.pos.x + dx;
                    const int weightY = tile.pos.y + dy;

                    if (!Utils::IsInMap(weightX, weightY, Map))
                    {
                        continue;
                    }
                    int distanceFromCity = tile.pos.distanceTo({ weightX, weightY });

                    // The desirability is increased in the tiles adjacent to the other cityTiles and decreased further
                    if (distanceFromCity == 1)
                    {
                        m_cityTilesDesirability[weightY * Map.width + weightX] *= 1.3f;
                    }
                    else
                    {
                        m_cityTilesDesirability[weightY * Map.width + weightX] /= weightRange + 2 - distanceFromCity;
                    }
                } 
            }
        }
    }

    for (Position& pos : tilesPositions)
    {
        m_cityTilesDesirability[pos.y * Map.width + pos.x] = -1.0f;
    }
}

int GameDatas::GetTimeOfDay() const
{
    return Turn % 40;
}
