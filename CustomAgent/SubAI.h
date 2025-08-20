#pragma once
template<typename TObject>
class SubAI
{
public:
	TObject& ManagedObject;

	SubAI(TObject& managedObject) :
		ManagedObject(managedObject)
	{

	}
};

