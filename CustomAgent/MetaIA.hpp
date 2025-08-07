#pragma once
#include <vector>

#include "../lux/city.hpp"
#include "../lux/game_objects.hpp"

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
    std::vector<lux::City> m_cities;
    std::vector<lux::Unit> m_units;

    std::vector<const lux::City&> GetNeedingCity() const;
    std::vector<const lux::Unit&> GetNeedingUnits() const;

    //Expand
    int NBUnitsToBuild() const;
    void MakeUnitsCollectResourcesForCity(const lux::City& city);
    void MakeUnitsCollectResourcesForThemselves(lux::Unit& unit);
    void BuildUnits();
    void BuildCities();
    void Research();
    
public:
    void Update(int turn);
};
