#pragma once
#include <vector>

#include "lux/city.hpp"
#include "lux/game_objects.hpp"

class MetaIA
{
    /*  
     *  Goals one after the other
     *  - Survive (keep units and city alive through each night)
     *  - Expand (more cities, more units, roads, research)
     *  - Attack (go annoy the ennemy, not necessary)
     */
private:
    // Survive
    std::vector<lux::City> m_cities;
    std::vector<lux::Unit> m_units;

    std::vector<const lux::City&> GetNeedingCity(int turn) const;
    std::vector<const lux::Unit&> GetNeedingUnits(int turn) const;

    //Expand
    
    
    bool CanBuildUnit() const;
    
public:
    void Update(int turn);
};
