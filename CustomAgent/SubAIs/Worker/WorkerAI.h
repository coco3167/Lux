#pragma once

#include "lux/game_objects.hpp"

#include "StateMachine/StateMachine.h"
#include "WorkerStateMachine.h"

#include "SubAIs/UnitAI.h"
#include "SubAIs/City/CityAI.h"

class CartAI;

class WorkerAI : public UnitAI
{
    typedef WorkerSM::WorkerSMInfos SMInfos;

public:
    static constexpr int FUEL_NEEDED_FOR_THE_NIGHT = 40;

private:
    StateMachine<WorkerSM::WorkerSMInfos> m_stateMachine;
    WorkerSM::WorkerSMInfos m_smInfos;

public:
    WorkerAI(lux::Unit* worker, GameDatas* gameDatas);

    virtual void BeginTurn() override;

    void Update();

    virtual void DrawDebug(GameDatas& gameDatas) override;

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

    bool RequestResources(CartAI* target, int maxResources);
    bool CanBuildCity() const;

    WorkerSM::Objective GetCurrentObjective() const;

private:

    std::unique_ptr<SMState<WorkerSM::WorkerSMInfos>> GetStartingState();
    int GetAvailableFuel() const;

    void TryTransfer(CartAI* target, ResourceType type, int& availableFuel, int& transferableReources);
};

