#pragma once
#include <vector>

#include "CityAI.h"
#include "../lux/city.hpp"
#include "../lux/game_objects.hpp"

#include "CityTileAI.h"
#include "UnitAI.h"

class MetaIA
{
    /*  
     *  Goals one after the other
     *  - Survive (keep units and city alive through each night)
     *  - Expand (more cities, more units, roads, research)
     *  - Attack (go annoy the ennemy, not necessary)
     */
private:
    constexpr int RESOURCE_PER_UNIT = 10;

    int m_turn = 0;
    std::string m_stringResult = "";
    
    // Survive
    std::vector<CityAI> m_cityAIs;
    std::vector<CityTileAI> m_cityTileAIs;
    std::vector<UnitAI> m_unitAIs;

    std::vector<CityAI&> GetNeedingCity();
    std::vector<UnitAI&> GetNeedingUnits();

    //Expand
    int NBUnitsToBuild() const;
    void MakeUnitsCollectResourcesForCity(CityAI& city);
    void MakeUnitsCollectResourcesForThemselves(UnitAI& unit);
    void BuildUnits();
    void BuildCities();
    void Research();
    
public:
    std::string Update(int turn);
};
