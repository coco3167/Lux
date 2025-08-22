#pragma once
#include <utility>

#include "GameDatas.h"


template<typename TObject>
class SubAI
{
public:
	TObject* ManagedObject;

	SubAI(TObject* managedObject) :
		ManagedObject(managedObject)
	{

	}

	virtual void DrawDebug(GameDatas& gameDatas)
	{

	}
};

