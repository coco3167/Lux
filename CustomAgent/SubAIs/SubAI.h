#pragma once
#include <utility>
#include <string>

#include "GameDatas.h"


template<typename TObject>
class SubAI
{
public:
	TObject* ManagedObject;
	std::string ManagedObjectID;

	SubAI(TObject* managedObject, std::string managedObjectID) :
		ManagedObject(managedObject),
		ManagedObjectID(std::move(managedObjectID))
	{

	}

	virtual void DrawDebug(GameDatas& gameDatas)
	{

	}
};

