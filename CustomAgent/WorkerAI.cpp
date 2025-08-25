#include "WorkerAI.h"

#include "../lux/annotate.hpp"

WorkerAI::WorkerAI(lux::Unit* worker, GameDatas* gameDatas) :
    SubAI(worker, worker->id),
    m_smInfos(worker, gameDatas),
    m_stateMachine(std::move(GetStartingState()))
{
}

void WorkerAI::Update()
{
    m_stateMachine.Update(m_smInfos);
}

void WorkerAI::DrawDebug(GameDatas& gameDatas)
{
    gameDatas.AddAction(std::move(Annotate::text(ManagedObject->pos.x, ManagedObject->pos.y, WorkerSM::WorkerSMUtils::ObjectiveToString(m_smInfos.CurrentObjective), 50)));
    m_stateMachine.DrawDebug(m_smInfos, *gameDatas.Actions);

    if (ManagedObject->team != gameDatas.Owner->team)
    {
        gameDatas.AddAction(std::move(Annotate::sidetext("INVALID OWNER TEAM")));
    }

    if (m_smInfos.SuppliedCity != nullptr && ManagedObject->team != m_smInfos.SuppliedCity->ManagedObject->team)
    {
        gameDatas.AddAction(std::move(Annotate::sidetext("INVALID CITY TEAM")));
    }
}

bool WorkerAI::NeedResources(int turn) const
{
    return false;
}

bool WorkerAI::IsAvailable() const
{
    return m_smInfos.CurrentObjective == WorkerSM::Objective::None;
}

void WorkerAI::CollectResources()
{
    m_smInfos.CurrentObjective = WorkerSM::Objective::CollectRessourceForSelf;
}

void WorkerAI::CollectResources(CityAI& cityAI)
{
    m_smInfos.CurrentObjective = WorkerSM::Objective::CollectRessourceForCity;
    m_smInfos.SuppliedCity = &cityAI;
}

void WorkerAI::BuildCityTile()
{
    m_smInfos.CurrentObjective = WorkerSM::Objective::BuildCity;
}

std::unique_ptr<SMState<WorkerSM::WorkerSMInfos>> WorkerAI::GetStartingState()
{
    return std::unique_ptr<WorkerSM::DefaultState>(new WorkerSM::DefaultState(m_smInfos));
}
