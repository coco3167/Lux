#include "WorkerAI.h"

#include "../lux/annotate.hpp"

WorkerAI::WorkerAI(lux::Unit* worker, GameDatas* gameDatas) :
    SubAI(worker),
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
    gameDatas.AddAction(std::move(Annotate::text(ManagedObject->pos.x, ManagedObject->pos.y, WorkerSM::ObjectiveToString(m_smInfos.CurrentObjective))));
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
    m_smInfos.SuppliedCity = cityAI.ManagedObject;
}

void WorkerAI::BuildCityTile()
{
    m_smInfos.CurrentObjective = WorkerSM::Objective::BuildCity;
}

std::unique_ptr<SMState<WorkerSM::WorkerSMInfos>> WorkerAI::GetStartingState()
{
    return std::unique_ptr<WorkerSM::DefaultState>(new WorkerSM::DefaultState(m_smInfos));
}
