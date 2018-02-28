#pragma once
#include "BackTrack.h"

class EventListener : public IGameEventListener2
{
public:
	EventListener();

	~EventListener();

	void FireGameEvent(IGameEvent* event) override;

	int GetEventDebugID() override;
};
