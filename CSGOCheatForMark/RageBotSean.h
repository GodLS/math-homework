#pragma once
#include "Singleton.h"
#include "valve_sdk/csgostructs.hpp"

class RageBotSean : public Singleton<RageBotSean>
{
public:
	void run(CUserCmd* pCmd);
	static C_BasePlayer* get_target(CUserCmd* pCmd);
	void aim_target(C_BasePlayer* target, CUserCmd* pCmd);
};
