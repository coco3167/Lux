#pragma once
#include <utility>

template<typename TObject>
class SubAI
{
public:
	TObject* ManagedObject;

	SubAI(TObject* managedObject) :
		ManagedObject(managedObject)
	{

	}
};

