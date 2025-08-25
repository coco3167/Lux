#include "CityAI.h"

CityAI::CityAI(lux::City* city) :
    SubAI(city, city->cityid)
{
}

bool CityAI::NeedResources(int turn)
{
    return ResourcesQuantityNeeded(turn) > 0;
}

float CityAI::ResourcesQuantityNeeded(int turn)
{
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

    // Turns to survive in the dark left
    int turnUsable = std::min(10,40 - turn % 40);
    
    return totalFuelNeeded * static_cast<float>(turnUsable) - ManagedObject->fuel;
}