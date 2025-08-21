#pragma once
#include <memory>

template<typename TSMInfos>
class SMState
{
public:

	virtual void StartState() {};
	virtual std::unique_ptr<SMState<TSMInfos>> UpdateState(TSMInfos& stateInfos) = 0;
	virtual void EndState() {};
};

