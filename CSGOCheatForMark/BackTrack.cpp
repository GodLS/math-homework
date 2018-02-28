#include "BackTrack.h"

#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

BacktrackData* bestBacktrackData = nullptr;

BackTrack::BackTrack()
{
	m_iLatestTick = g_GlobalVars->tickcount;
}

void BackTrack::CreateMove_Post(C_BasePlayer* pLocal, CUserCmd* pCmd)
{
	if (/*!Options::g_bBackTrackingEnabled ||*/ !pLocal || !pLocal->IsAlive())
		return;

	const auto bestTargetIndex = UpdateBestTarget(pLocal, pCmd);
	if (bestTargetIndex == -1)
		return;

	auto tempFloat = FLT_MAX;
	const auto viewDirection = pCmd->viewangles + pLocal->m_aimPunchAngle() * 2.f;
	for (auto t = 0; t < 12; ++t)
	{
		//const auto tempFovDistance = Utils::DistancePointToLine(m_headPositions[bestTargetIndex][t].aimTargetPos, pLocal->GetEyePos(), viewDirection);
		auto tempFov = Utils::GetFov(viewDirection, Math::CalcAngle(g_LocalPlayer->GetEyePos(), m_headPositions[bestTargetIndex][t].aimTargetPos),bestTargetIndex );
		if (tempFloat > tempFov && m_headPositions[bestTargetIndex][t].simTime > pLocal->m_flSimulationTime() - 1)
		{
			tempFloat = tempFov;
			bestBacktrackData = &m_headPositions[bestTargetIndex][t];
		}
	}
	if (pCmd->buttons & IN_ATTACK && g_LocalPlayer->CanAttack() && bestBacktrackData)
	{
		pCmd->tick_count = TIME_TO_TICKS(bestBacktrackData->simTime);
		//Utils::ConsolePrint("%i ticks\n", TIME_TO_TICKS(pLocal->m_flSimulationTime()) - TIME_TO_TICKS(bestBacktrackData->simTime));
		//pCmd->tick_count = static_cast<int>(bestBacktrackData->simTime / g_GlobalVars->interval_per_tick);
	}
}
void BackTrack::OnLocalPlayerSpawn(bool inrestart)
{
	memset(&m_headPositions[0][0], 0, sizeof BacktrackData * 768);
}

void BackTrack::UpdateRecord(int i)
{
	auto pEntity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
	if (pEntity && pEntity->IsAlive() && !pEntity->IsDormant())
	{
		const auto lby = pEntity->m_flLowerBodyYawTarget();
		if (lby != m_records[i].lby)
		{
			m_records[i].tickCount = m_iLatestTick;
			m_records[i].lby = lby;
			m_records[i].headPosition = pEntity->GetHitboxPos(HITBOX_HEAD);

		}
	}
	else
	{
		m_records[i].tickCount = 0;
	}
}


bool BackTrack::IsTickValid(int tick) const
{
	const auto deltaTime = (m_iLatestTick - tick) * g_GlobalVars->interval_per_tick;
	return fabs(deltaTime) <= 0.2f;
}

void BackTrack::FrameStageNotify_Post(ClientFrameStage_t stage)
{
	if (stage != ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		return;

	m_iLatestTick = g_GlobalVars->tickcount;
	for (auto i = 0; i < 64; i++)
		UpdateRecord(i);
}

int BackTrack::UpdateBestTarget(C_BasePlayer* pLocal, CUserCmd* pCmd)
{
	if (!pLocal)
		return -1;

	auto bestTargetIndex = -1;
	auto bestFov = FLT_MAX;
	player_info_t info;
	for (auto i = 0; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto pTarget = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (!pTarget || pTarget == pLocal || pTarget->IsDormant() || !pTarget->IsAlive() || pTarget->m_bGunGameImmunity())
			continue;

		if (!g_EngineClient->GetPlayerInfo(i, &info))
			continue;

		if (pTarget->m_iTeamNum() == pLocal->m_iTeamNum() /*&& !Options::g_bDeathmatch*/)
			continue;

		const auto hitboxPos = pTarget->GetHitboxPos(HITBOX_HEAD);

		m_headPositions[i][pCmd->command_number % 13] = BacktrackData
		{
			pTarget->m_flSimulationTime(),
			hitboxPos
		};
		const auto viewDirection = pCmd->viewangles + pLocal->m_aimPunchAngle() * 2.f;
		const auto fovDistance = Utils::DistancePointToLine(hitboxPos, pLocal->GetEyePos(), viewDirection);

		if (bestFov > fovDistance)
		{
			bestFov = fovDistance;
			bestTargetIndex = i;
		}
	}
	return bestTargetIndex;
}
