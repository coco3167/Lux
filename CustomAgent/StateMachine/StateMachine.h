#pragma once
#include <memory>
#include "SMState.h"


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
		std::unique_ptr<SMState<TSMInfos>> requestedState = m_currentState->UpdateState(stateInfos);
		if (requestedState) 
		{
			ChangeState(std::move(requestedState));
		}
	}

	void ChangeState(std::unique_ptr<SMState<TSMInfos>> newState) 
	{
		m_currentState->EndState();
		m_currentState = std::move(newState);
		m_currentState->StartState();
	}

	virtual void DrawDebug(std::vector<std::string>& actions)
	{
		m_currentState->DrawDebug(actions);
	}
};

