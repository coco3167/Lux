#include "SubAIs/City/CityAI.h"
#include "Utils/Utils.hpp"
#include "Utils/Debug/Debug.h"

CityAI::CityAI(lux::City* city, GameDatas* gameDatas) :
    SubAI(city, city->cityid), 
    m_gameDatas(gameDatas)
{
}

bool CityAI::NeedResources(int turn)
{
    return ResourcesQuantityNeeded(turn) > 0;
}

float CityAI::ResourcesQuantityNeeded(int turn)
{
    if (m_gameDatas->TurnsUntilNight() > 25)
    {
        return 0.0f;
    }

    float totalFuelNeeded = 0;

    // Get all cityTiles in an unordered set for faster access
    m_unorderedCityTilePositions.clear();

    for (lux::CityTile& cityTile : ManagedObject->citytiles)
    {
        m_unorderedCityTilePositions.insert(cityTile.pos);
    }
    
    for (const lux::CityTile& cityTileAi : ManagedObject->citytiles)
    {
        float fuelNeeded = 23;
        lux::Position position = cityTileAi.pos;
        for (lux::DIRECTIONS direction : lux::ALL_DIRECTIONS)
        {
            lux::Position newPosition = position.translate(direction, 1);
            if(m_unorderedCityTilePositions.find(newPosition) != m_unorderedCityTilePositions.end())
            {
                fuelNeeded -= 5;
            }
        }

        totalFuelNeeded += fuelNeeded;
    }

    totalFuelNeeded *= 15.0f;

    Debug::Log(Utils::FormatString("City Upkeep : %f", totalFuelNeeded));
    
    return totalFuelNeeded - ManagedObject->fuel;
}