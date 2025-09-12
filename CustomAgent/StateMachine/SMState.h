#pragma once
#include <memory>
#include <vector>
#include <string>


template<typename TSMInfos>
class SMState
{
public:

	virtual void StartState() {};
	virtual std::unique_ptr<SMState<TSMInfos>> UpdateState(TSMInfos& stateInfos) = 0;
	virtual void EndState() {};

	virtual void DrawDebug(TSMInfos& stateInfos) { }
};

