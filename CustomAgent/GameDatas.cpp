#include "GameDatas.h"

#include <algorithm>

#include "PathFinder.hpp"

GameDatas::GameDatas(GameMap& map, Player* owner) :
    Map(map),
    Actions(nullptr),
    Owner(owner),
    m_cityTilesDesirability(map.height * map.width)
{
}

void GameDatas::Update(std::vector<string>* actions, Player* owner)
{
    Actions = actions;
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

Cell *GameDatas::GetClosestResourceCell(Position startPosition) const
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
            tileDesirability = m_cityTilesDesirability[y * Map.width + x] * GetDistanceDesirabilityFactor(startPosition, {x, y});
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
    std::vector<DIRECTIONS> path = {};
    path.reserve(10);

    PathFinder::FindPath(Map, startPosition, targetPosition, *Owner, path);
    int pathLength = path.size();

    return static_cast<float>(std::max(-std::log(pathLength) / 2.0f, 0.0));
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

    
    const int weightRange = 2;
    for (const Cell* resourceTile : m_resourceTiles)
    {
        float resourceDesirability;
        switch (resourceTile->resource.type)
        {
            case ResourceType::wood:
                resourceDesirability = 10.0f;

            case ResourceType::coal: 
                if (!Owner->researchedCoal())
                {
                    continue;
                }
                resourceDesirability = 100.0f;

            case ResourceType::uranium: 
                if (!Owner->researchedUranium())
                {
                    continue;
                }
                resourceDesirability = 1000.0f;
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

                if (Map.getCell(weightX, weightY)->hasResource()) 
                {
                    continue;
                }

                m_cityTilesDesirability[weightY * Map.width + weightX] += resourceDesirability;
            }
        }
    }
}
