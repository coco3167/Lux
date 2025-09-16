#include "SubAIs/Worker/WorkerAI.h"

#include "lux/annotate.hpp"
#include "SubAIs/Cart/CartAI.hpp"

#include "Utils/Debug/Debug.h"

WorkerAI::WorkerAI(lux::Unit* worker, GameDatas* gameDatas) :
    UnitAI(worker),
    m_smInfos(this, gameDatas),
    m_stateMachine(std::move(GetStartingState()))
{
    Debug::Log("[WorkerAI] Ctor");
}

void WorkerAI::BeginTurn()
{
    UnitAI::BeginTurn();

    // To prevent multiple workers from building cities at the same position
    if (m_smInfos.CurrentObjective == WorkerSM::Objective::BuildCity)
    {
        m_smInfos.Datas->NullifyCityTileDesirability(m_smInfos.TargetPosition);
    }
}

void WorkerAI::Update()
{
    Debug::Log(Utils::FormatString("[WorkerAI] Update Worker %s at adress %ld", ManagedObject->id.c_str(), (long)this));
    Debug::Log(Utils::FormatString("[WorkerAI] Worker Objective : %s", WorkerSM::WorkerSMUtils::ObjectiveToString(m_smInfos.CurrentObjective).c_str()));
    
    
    m_stateMachine.Update(m_smInfos);
}

void WorkerAI::DrawDebug(GameDatas& gameDatas)
{
    gameDatas.AddAction(std::move(Annotate::text(ManagedObject->pos.x, ManagedObject->pos.y, WorkerSM::WorkerSMUtils::ObjectiveToString(m_smInfos.CurrentObjective), 50)));
    m_stateMachine.DrawDebug(m_smInfos);

    if (ManagedObject->team != gameDatas.Owner->team)
    {
        gameDatas.AddAction(std::move(Annotate::sidetext("[WorkerAI] INVALID OWNER TEAM")));
    }


    CityAI* suppliedCity = m_smInfos.GetSuppliedCity();
    if (suppliedCity != nullptr && ManagedObject->team != suppliedCity->ManagedObject->team)
    {
        gameDatas.AddAction(std::move(Annotate::sidetext("[WorkerAI] INVALID CITY TEAM")));
    }
}

bool WorkerAI::NeedResources(int turn) const
{
    if (m_smInfos.Datas->TurnsUntilNight() > 10)
    {
        return false;
    }

    return GetAvailableFuel() < 0;
}

bool WorkerAI::IsAvailable() const
{
    return m_smInfos.CurrentObjective == WorkerSM::Objective::None;
}

void WorkerAI::CollectResources()
{
    Debug::Log(Utils::FormatString("[WorkerAI] Ordered Worker %s to collect resources", ManagedObject->id.c_str()));
    m_smInfos.CurrentObjective = WorkerSM::Objective::CollectResourceForSelf;
}

void WorkerAI::CollectResources(CityAI& cityAI)
{
    Debug::Log(Utils::FormatString("[WorkerAI] Ordered Worker %s to collect resources for city %s", ManagedObject->id.c_str(), cityAI.ManagedObject->cityid.c_str()));
    m_smInfos.CurrentObjective = WorkerSM::Objective::CollectResourceForCity;
    m_smInfos.SuppliedCityID = cityAI.ManagedObjectID;
    m_smInfos.AlreadyCollectedResources = false;
}

void WorkerAI::BuildCityTile()
{
    Debug::Log(Utils::FormatString("[WorkerAI] Ordered Worker %s to build city", ManagedObject->id.c_str()));
    m_smInfos.CurrentObjective = WorkerSM::Objective::BuildCity;
}

bool WorkerAI::RequestResources(CartAI* target, int maxResources)
{
    if (NeedResources(m_smInfos.Datas->Turn))
    {
        return false;
    }

    int availableFuel = GetAvailableFuel();
    int transferableResources = maxResources;

    TryTransfer(target, ResourceType::uranium, availableFuel, transferableResources);
    TryTransfer(target, ResourceType::coal, availableFuel, transferableResources);
    TryTransfer(target, ResourceType::wood, availableFuel, transferableResources);

    return true;
}

bool WorkerAI::CanBuildCity() const
{
    return ManagedObject->getCargoSpaceLeft() == 0;
}

WorkerSM::Objective WorkerAI::GetCurrentObjective() const
{
    return m_smInfos.CurrentObjective;
}

std::unique_ptr<SMState<WorkerSM::WorkerSMInfos>> WorkerAI::GetStartingState()
{
    return std::unique_ptr<WorkerSM::DefaultState>(new WorkerSM::DefaultState(m_smInfos));
}

int WorkerAI::GetAvailableFuel() const
{
    int fuel = Utils::GetFuel(&ManagedObject->cargo);
    const int neededUpkeep = (int) GAME_CONSTANTS["PARAMETERS"]["LIGHT_UPKEEP"]["WORKER"] * (int) GAME_CONSTANTS["PARAMETERS"]["NIGHT_LENGTH"];
    return fuel - neededUpkeep;
}

void WorkerAI::TryTransfer(CartAI* target, ResourceType type, int& availableFuel, int& transferableReources)
{
    int fuelPerResource;
    int resourcesInCargo;

    switch (type)
    {
    case ResourceType::uranium:
        fuelPerResource = 40;
        resourcesInCargo = ManagedObject->cargo.uranium;

    case ResourceType::coal:
        fuelPerResource = 10;
        resourcesInCargo = ManagedObject->cargo.coal;

    case ResourceType::wood:
        fuelPerResource = 1;
        resourcesInCargo = ManagedObject->cargo.wood;
    }

    int availableResources = std::min(availableFuel / fuelPerResource, std::min(resourcesInCargo, transferableReources));
    if (availableResources > 0)
    {
        availableFuel -= availableResources * fuelPerResource;
        transferableReources -= availableResources;
        m_smInfos.Datas->AddAction(std::move(ManagedObject->transfer(ManagedObject->id, target->ManagedObject->id, type, availableResources)));
    }
}
