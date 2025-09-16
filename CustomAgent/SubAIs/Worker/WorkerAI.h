#pragma once

#include "lux/game_objects.hpp"

#include "StateMachine/StateMachine.h"
#include "WorkerStateMachine.h"

#include "SubAIs/UnitAI.h"
#include "SubAIs/City/CityAI.h"

class CartAI;

/// <summary>
/// A wrapper arround a Worker to which we can give orders it will follow for multiple turns
/// </summary>
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

    /// <summary>
    /// Returns wether the managed worker can make an action this turn
    /// </summary>
    bool IsAvailable() const;

    /// <summary>
    /// Orders the managed worker to go gather resources for itself
    /// </summary>
    void CollectResources();

    /// <summary>
    /// Orders the managed worker to go gather resources and then go back to the given city
    /// </summary>
    void CollectResources(CityAI& cityAI);

    /// <summary>
    /// Orders the managed worker to go build a new city tile
    /// </summary>
    void BuildCityTile();

    /// <summary>
    /// Asks the worker to transfer resources to the given cart
    /// </summary>
    bool RequestResources(CartAI* target, int maxResources);


    /// <summary>
    /// Returns wether the worker has enough resources to build a city tile
    /// </summary>
    bool CanBuildCity() const;

    /// <summary>
    /// Returns the current objective of the worker
    /// </summary>
    WorkerSM::Objective GetCurrentObjective() const;

private:

    std::unique_ptr<SMState<WorkerSM::WorkerSMInfos>> GetStartingState();
    int GetAvailableFuel() const;

    void TryTransfer(CartAI* target, ResourceType type, int& availableFuel, int& transferableReources);
};

