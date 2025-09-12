#pragma once
#include <memory>
#include "SMState.h"

#include "Utils/Debug/Debug.h"


template<typename TSMInfos>
class StateMachine
{
private:
	std::unique_ptr<SMState<TSMInfos>> m_currentState;

public:
	StateMachine(std::unique_ptr<SMState<TSMInfos>> startState) :
		m_currentState(std::move(startState))
	{
		m_currentState->StartState();
	}

	void Update(TSMInfos& stateInfos) 
	{
        Debug::Log("[SM] Update current state");
		std::unique_ptr<SMState<TSMInfos>> requestedState = m_currentState->UpdateState(stateInfos);
		if (requestedState) 
		{
        	Debug::Log("[SM] Changing state");
			ChangeState(std::move(requestedState));
		}
	}

	void ChangeState(std::unique_ptr<SMState<TSMInfos>> newState) 
	{
		m_currentState->EndState();
		m_currentState = std::move(newState);
		m_currentState->StartState();
	}

	virtual void DrawDebug(TSMInfos& stateInfos)
	{
		m_currentState->DrawDebug(stateInfos);
	}
};

