#pragma once
#include <vector>

#include "../lux/city.hpp"
#include "../lux/game_objects.hpp"

#include "CityAI.h"
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
    
    // Survive
    std::vector<CityAI> m_cityAIs;
    std::vector<UnitAI> m_unitAIs;

    std::vector<CityAI&> GetNeedingCity() const;
    std::vector<UnitAI&> GetNeedingUnits() const;

    //Expand
    int NBUnitsToBuild() const;
    void MakeUnitsCollectResourcesForCity(CityAI& city);
    void MakeUnitsCollectResourcesForThemselves(UnitAI& unit);
    void BuildUnits();
    void BuildCities();
    void Research();
    
public:
    void Update(int turn);
};
