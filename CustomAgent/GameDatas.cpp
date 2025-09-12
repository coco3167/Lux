#include "GameDatas.h"

#include <algorithm>

#include "Utils/PathFinder.hpp"

#include "SubAIs/Worker/WorkerAI.h"
#include "SubAIs/Cart/CartAI.hpp"

#include "Utils/Utils.hpp"
#include "Utils/Debug/Debug.h"

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

void GameDatas::NullifyCityTileDesirability(const Position& position)
{
    m_cityTilesDesirability[position.y * Map.width + position.x] = 0.0f;
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
            //Debug::Log(Utils::FormatString("CityTile desirability x:%i y:%i | Score : %f", x, y, tileDesirability));

            if (tileDesirability > mostDesirableCityTileScore)
            {
                mostDesirableCityTileScore = tileDesirability;
                mostDesirableCityTile = Map.getCell(x, y);
            }
        }
    }

    Debug::Log(Utils::FormatString("Most Desirable CityTile x:%i y:%i | Score : %f", mostDesirableCityTile->pos.x, mostDesirableCityTile->pos.y, mostDesirableCityTileScore));

    return mostDesirableCityTile;

}

float GameDatas::GetDistanceDesirabilityFactor(Position startPosition, Position targetPosition) const
{
    // std::vector<DIRECTIONS> path = {};
    // path.reserve(10);

    // PathFinder::FindPath(Map, startPosition, targetPosition, Owner, path);
    int pathLength = startPosition.distanceTo(targetPosition);

    return static_cast<float>(Utils::Clamp(1.0 - std::log10(pathLength) / 2.0, 0.0001, 1.0));
}

WorkerAI* GameDatas::GetClosestWorker(Position startPosition, WorkerSM::Objective desiredObjective) const
{
    WorkerAI* closestWorker = nullptr;
    float closestDist = 9999999.0f;
    for (auto it = WorkerAIs->begin(); it != WorkerAIs->end(); it++)
    {
        WorkerAI* worker = it->get();

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

std::vector<Position> GameDatas::GetNextTurnPositions() const
{
    std::vector<Position> nextTurnPositions{};
    nextTurnPositions.reserve(WorkerAIs->size() + CartAIs->size());

    for (int i = 0; i < WorkerAIs->size(); ++i)
    {
        nextTurnPositions.push_back((*WorkerAIs)[i]->PositionNextTurn);
    }
    for (int i = 0; i < CartAIs->size(); ++i)
    {
        nextTurnPositions.push_back((*CartAIs)[i]->PositionNextTurn);
    }

    return nextTurnPositions;
}

bool GameDatas::PositionAvailableNextTurn(Position position) const
{
    if (!Utils::IsInMap(position, Map))
    {
        return false;
    }

    for (int i = 0; i < WorkerAIs->size(); ++i)
    {
        if ((*WorkerAIs)[i]->PositionNextTurn == position)
        {
            return false;
        }
    }

    for (int i = 0; i < CartAIs->size(); ++i)
    {
        if ((*CartAIs)[i]->PositionNextTurn == position)
        {
            return false;
        }
    }
    return true;
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

    //Debug::LogWarning(Utils::FormatString("ResourceTile Count : %i", m_resourceTiles.size()));
}

void GameDatas::FillCityTilesDesirability()
{
    for (int x = 0; x < Map.width; ++x)
    {
        for (int y = 0; y < Map.height; ++y)
        {
            m_cityTilesDesirability[y * Map.width + x] = GetTileBaseCityDesirability(Map.getCell(x, y));
        }
    }

    ApplyResourceDesirability();
    ApplyCityProximityDesirability();
}

float GameDatas::GetTileBaseCityDesirability(Cell* cell) const
{
    if (cell->hasResource())
    {
        return 0.0f;
    }

    if (cell->citytile != nullptr)
    {
        return 0.0f;
    }
    return 100.0f;
}

void GameDatas::ApplyResourceDesirability()
{
    const int weightRange = 1;
    for (const Cell* resourceTile : m_resourceTiles)
    {
        float resourceDesirability;
        switch (resourceTile->resource.type)
        {
        case ResourceType::wood:
            resourceDesirability = 1.5f;
            break;

        case ResourceType::coal:
            if (!Owner->researchedCoal())
            {
                continue;
            }
            resourceDesirability = 2.25f;
            break;

        case ResourceType::uranium:
            if (!Owner->researchedUranium())
            {
                continue;
            }
            resourceDesirability = 3.5f;
            break;
        }

        for (DIRECTIONS dir : ALL_DIRECTIONS)
        {
            Position neighbouringPosition = resourceTile->pos.translate(dir, 1);
            if (!Utils::IsInMap(neighbouringPosition, Map))
            {
                continue;
            }

            m_cityTilesDesirability[neighbouringPosition.y * Map.width + neighbouringPosition.x] *= resourceDesirability;
        }
         
    }
}

void GameDatas::ApplyCityProximityDesirability()
{
    const int weightRange = 1;

    for (std::map<string, City>::iterator it = Owner->cities.begin(); it != Owner->cities.end(); it++)
    {
        City* city = &it->second;
        for (CityTile& tile : city->citytiles)
        {
            for (DIRECTIONS dir : ALL_DIRECTIONS)
            {
                Position neighbouringPosition = tile.pos.translate(dir, 1);
                if (!Utils::IsInMap(neighbouringPosition, Map))
                {
                    continue;
                }

                m_cityTilesDesirability[neighbouringPosition.y * Map.width + neighbouringPosition.x] *= 3.0f;
            }
        }
    }
}

int GameDatas::GetTimeOfDay() const
{
    return Turn % 40;
}
