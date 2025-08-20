#pragma once
#include "SubAI.h"

#include "../lux/game_objects.hpp"

#include "StateMachine/StateMachine.h"
#include "WorkerStateMachine.h"

#include "CityAI.h"

class WorkerAI : public SubAI<lux::Unit>
{
    typedef WorkerSM::WorkerSMInfos SMInfos;

private:
    StateMachine<WorkerSM::WorkerSMInfos> m_stateMachine;
    WorkerSM::WorkerSMInfos m_smInfos;

public:
    WorkerAI(lux::Unit* worker, GameDatas* gameDatas);

    void Update();

    // TODO checks if the unit needs more resources at the current turn
    bool NeedResources(int turn) const;

    // TODO checks if the unit can do smthing
    bool IsAvailable() const;

    // TODO Collect resources for themselves
    void CollectResources();

    // TODO Collect resources for a city
    void CollectResources(CityAI& cityAI);

    // TODO build city at the best place
    void BuildCityTile();


    std::unique_ptr<SMState<WorkerSM::WorkerSMInfos>> GetStartingState();
};

