#include "WorkerAI.h"

WorkerAI::WorkerAI(lux::Unit& worker, GameDatas gameDatas) :
	Worker(worker),
	m_stateMachine(std::move(GetStartingState())),
	m_smInfos(worker, gameDatas)
{
}

void WorkerAI::Update()
{
	m_stateMachine.Update(m_smInfos);
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
	m_smInfos.SuppliedCity = &cityAI.City;
}

void WorkerAI::BuildCityTile()
{
	m_smInfos.CurrentObjective = WorkerSM::Objective::BuildCity;
}

std::unique_ptr<SMState<WorkerSM::WorkerSMInfos>> WorkerAI::GetStartingState()
{
	return std::make_unique<WorkerSM::DefaultState>();
}
