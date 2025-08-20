#include "CityAI.h"

CityAI::CityAI(lux::City& city) :
    SubAI(city)
{
}

bool CityAI::NeedResources(int turn) const
{
    return false;
}

bool CityAI::IsAvailable() const
{
    return false;
}

int CityAI::ResourcesQuantityNeeded(int turn) const
{
    return 0;
}

int CityAI::UnitBuildScore() const
{
    return 0;
}

void CityAI::BuildUnit()
{
}

void CityAI::Research()
{
}
