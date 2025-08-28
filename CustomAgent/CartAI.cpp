#include "CartAI.hpp"

#include <cmath>

#include "Utils.hpp"
#include "WorkerAI.h"

#include "../lux/annotate.hpp"
#include "Annotator.hpp"

CartAI::CartAI(lux::Unit* cart, GameDatas* gameDatas) :
	SubAI(cart, cart->id),
	m_gameDatas(gameDatas),
	m_hasDestination(false)
{
	state = CartState::STANDBY;
	requestTarget = nullptr;
}

void CartAI::Update()
{
	UpdateDestination();

	Move();

	if (DestinationReached())
	{
		m_hasDestination = false;

		switch (state)
		{
		case CartState::STANDBY: 
			return;

		// Turn around and continue to upgrade the road until further notice.
		case CartState::BUILDING_ROAD:
		{
			//origin, destination = destination, origin;
			Position tmp = origin;
			origin = destination;
			destination = tmp;
			m_hasDestination = true;
			break;
		}

		case CartState::REQUEST_FROM_UNIT:
			if (requestTarget->RequestResources(this, ManagedObject->getCargoSpaceLeft()))
			{
				GoResupplyClosestCityTile(resupplyTarget);
				break;
			}
			// If the worker refuses to give resources 
			state = CartState::STANDBY;
			break;

		case CartState::RESUPPLYING_CITY:
			// TODO Transfer cargo
			state = CartState::STANDBY;
			break;
		}
	}
}

bool CartAI::NeedResources(int turn) const
{
	return false;
}

bool CartAI::IsAvailable() const
{
	return state == CartState::STANDBY;
}

bool CartAI::DestinationReached() const
{
	if (destination.x == -1) return false;
	return destination == ManagedObject->pos;
}

void CartAI::UpdateDestination()
{
	if (state != REQUEST_FROM_UNIT)
	{
		return;
	}

	destination = Utils::GetClosestAdjacentTile(ManagedObject->pos, requestTarget->ManagedObject->pos, m_gameDatas->Map);
	m_hasDestination = true;
}

void CartAI::Move()
{
	if (!m_hasDestination)
	{
		return;
	}

	std::vector<DIRECTIONS> pathToTarget{};
	pathToTarget.reserve(10);

	bool pathFound = PathFinder::FindPath(m_gameDatas->Map, ManagedObject->pos, destination, m_gameDatas->Owner, pathToTarget);

	if (!pathFound)
	{
		m_gameDatas->AddAction(std::move(Annotate::text(ManagedObject->pos.x, ManagedObject->pos.y, "NO PATH", 40)));
		return;
	}

	Annotator::TracePath(ManagedObject->pos, pathToTarget, *m_gameDatas->Actions);

	//m_gameDatas->AddAction(std::move(Annotate::circle(ManagedObject->pos.x, ManagedObject->pos.y)));
	m_gameDatas->AddAction(std::move(Annotate::x(destination.x, destination.y)));

	m_gameDatas->AddAction(std::move(ManagedObject->move(pathToTarget[0])));
}

void CartAI::GoRequestFromUnit(WorkerAI* unit)
{
	state = REQUEST_FROM_UNIT;
	requestTarget = unit;
	UpdateDestination();
}

void CartAI::GoResupplyClosestCityTile(CityAI* city)
{
	state = RESUPPLYING_CITY;
	const CityTile* closestTile = PathFinder::GetClosestCityTile(ManagedObject->pos, city->ManagedObject, m_gameDatas->Map, m_gameDatas->Owner);
	destination = closestTile->pos;
	m_hasDestination = true;
}

bool CartAI::TryGoResupply(CityAI* city)
{
	if (ManagedObject->getCargoSpaceLeft() == 0)
	{
		WorkerAI* closestWorker = m_gameDatas->GetClosestWorker(ManagedObject->pos, WorkerSM::Objective::CollectRessourceForCity);
		if (closestWorker == nullptr)
		{
			state = STANDBY;
			return false;
		}
		GoRequestFromUnit(closestWorker);
		resupplyTarget = city;
	}
	else
	{
		GoResupplyClosestCityTile(city);
	}
	return true;
}

void CartAI::BuildRoad(lux::Position start, lux::Position end)
{
	state = BUILDING_ROAD;
	origin = start;
	destination = end;
	m_hasDestination = true;
}

void CartAI::Transfer(lux::Unit& unit)
{
	int fuel = Utils::GetFuel(&ManagedObject->cargo);

	// If we have surplus, we can give some to the unit for it to survive.
	int fuelToTransfer = unit.isWorker() ? 
		WorkerAI::FUEL_NEEDED_FOR_THE_NIGHT : 
		CartAI::FUEL_NEEDED_FOR_THE_NIGHT;

	// We don't have enough fuel for both the cart and the unit, abort transfer.
	if (fuel < FUEL_NEEDED_FOR_THE_NIGHT + fuelToTransfer)
	{
		return;
	}

	int cargoU = ManagedObject->cargo.uranium;
	if (cargoU > 0)
	{
		int uAmmount;
		if (cargoU > fuelToTransfer)
		{
			uAmmount = static_cast<int>(std::floor(fuelToTransfer / 10));
			cargoU -= uAmmount;
		}
		else
		{
			uAmmount = cargoU;
			cargoU = 0;
		}
		fuelToTransfer -= uAmmount;
		m_gameDatas->AddAction(std::move(ManagedObject->transfer(ManagedObject->id, unit.id, lux::ResourceType::coal, uAmmount)));
	}

	int cargoCoal = ManagedObject->cargo.coal;
	if (cargoCoal > 0)
	{
		int coalAmmount;
		if (cargoCoal > fuelToTransfer)
		{
			coalAmmount = static_cast<int>(std::floor(fuelToTransfer / 10));
			cargoCoal -= coalAmmount;
		}
		else
		{
			coalAmmount = cargoCoal;
			cargoCoal = 0;
		}
		fuelToTransfer -= coalAmmount;
		m_gameDatas->AddAction(std::move(ManagedObject->transfer(ManagedObject->id, unit.id, lux::ResourceType::coal, coalAmmount)));

		if (fuelToTransfer <= 0)
		{
			return;
		}

	}

	int cargoWood = ManagedObject->cargo.wood;
	if (cargoWood > 0)
	{
		int woodAmmount;
		if (cargoWood > fuelToTransfer)
		{
			woodAmmount = fuelToTransfer;
			cargoWood -= woodAmmount;
		}
		else
		{
			woodAmmount = cargoWood;
			cargoWood = 0;
		}
		ManagedObject->cargo.wood = cargoWood;

		m_gameDatas->AddAction(std::move(ManagedObject->transfer(ManagedObject->id, unit.id, lux::ResourceType::wood, woodAmmount)));

		fuelToTransfer -= woodAmmount;
		if (fuelToTransfer <= 0)
		{
			return;
		}
	}
}