#include <cmath>

#include "CartAI.hpp"
#include "Utils.hpp"

CartAI::CartAI(lux::Unit& cart, lux::GameMap& map) :
	cart(cart), map(map)
{
	state = CartState::STANDBY;
	resupplyTarget = nullptr;
}

void CartAI::Update()
{
	if (state == RESUPPLYING_UNIT)
		UpdateDestination();

	if (DestinationReached())
	{
		switch (state)
		{
		case CartState::STANDBY: return;
			// Turn around and continue to upgrade the road until further notice.
		case CartState::BUILDING_ROAD:
		{
			Position tmp = origin;
			origin = destination;
			destination = tmp;
			break;
		}
			
		case CartState::RESUPPLYING_UNIT:
			// TODO Transfer cargo
			//string cmd = cart.transfer(cart.id, resupplyTarget.id,)
		case CartState::RESUPPLYING_CITY:
			// TODO Transfer cargo
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
	return false;
}

bool CartAI::DestinationReached()
{
	if (destination.x = -1) return false;
	return destination == cart.pos;
}

void CartAI::UpdateDestination()
{
	destination = Utils::GetClosestAdjacentTile(cart.pos, resupplyTarget->pos, map);
}

void CartAI::Resupply(lux::Unit& unit)
{
	state = RESUPPLYING_UNIT;
	resupplyTarget = &unit;
	UpdateDestination();
}

void CartAI::Resupply(lux::CityTile& city)
{
	state = RESUPPLYING_CITY;
	destination = city.pos;
}

void CartAI::BuildRoad(lux::Position start, lux::Position end)
{
	state = BUILDING_ROAD;
	origin = start;
	destination = end;
}

std::vector<std::string> CartAI::Transfer(lux::Unit unit)
{
	std::vector<std::string> res = {};
	int fuel = cart.cargo.wood + cart.cargo.coal * 10 + cart.cargo.uranium * 40;

	// If we have surplus, we can give some to the unit for it to survive.
	int fuelToTransfer = unit.isWorker() ? UnitAI::FUEL_NEEDED_FOR_THE_NIGHT
										 : CartAI::FUEL_NEEDED_FOR_THE_NIGHT;

	// We don't have enough fuel for both the cart and the unit, abort transfer.
	if (fuel < FUEL_NEEDED_FOR_THE_NIGHT + fuelToTransfer) return;
	
	int cargoU = cart.cargo.uranium;
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
		res.push_back(cart.transfer(cart.id, unit.id,
			lux::ResourceType::coal, uAmmount));
	}

	int cargoCoal = cart.cargo.coal;
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
		res.push_back(cart.transfer(cart.id, unit.id,
			lux::ResourceType::coal, coalAmmount));
		if (fuelToTransfer <= 0) return res;
	}

	int cargoWood = cart.cargo.wood;
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
		cart.cargo.wood = cargoWood;
		res.push_back(cart.transfer(cart.id, unit.id,
									lux::ResourceType::wood, woodAmmount));
		fuelToTransfer -= woodAmmount;
		if (fuelToTransfer <= 0) return res;
	}
	
	return res;
}