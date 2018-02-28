#include "BHOP.h"
#include "valve_sdk/csgostructs.hpp"

void BHOP::bhop(CUserCmd* pCmd)
{
	if (g_LocalPlayer->m_MoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_MoveType() == MOVETYPE_NOCLIP)
		return;

	static bool bJumped = false;
	static bool bFakeJump = false;

	if (!bJumped && bFakeJump)
	{
		bFakeJump = false;
		pCmd->buttons |= IN_JUMP;
	}
	else if (pCmd->buttons & IN_JUMP)
	{
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		{
			bJumped = true;
			bFakeJump = true;
		}
		else
		{
			pCmd->buttons &= ~IN_JUMP;
			bJumped = false;
		}
	}
	else
	{
		bJumped = false;
		bFakeJump = false;
	}
}
