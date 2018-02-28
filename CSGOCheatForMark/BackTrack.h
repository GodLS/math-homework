#pragma once
#include "valve_sdk/Math/Vector.hpp"
#include "valve_sdk/Misc/CUserCmd.hpp"
#include "Utils.h"
#include "Math.h"
#include "valve_sdk/csgostructs.hpp"

struct LBYRecords
{
	int tickCount;
	float lby;
	Vector headPosition;
};

struct BacktrackData
{
	float simTime;
	Vector aimTargetPos;
};
extern BacktrackData* bestBacktrackData;

class C_BasePlayer;

class BackTrack : public Singleton<BackTrack>
{
public:
	BackTrack();

	void CreateMove_Post(C_BasePlayer* pLocal, CUserCmd* pCmd);

	void OnLocalPlayerSpawn(bool inrestart);

	void FrameStageNotify_Post(ClientFrameStage_t stage);

private:
	BacktrackData m_headPositions[64][12];

	int m_iLatestTick;

	LBYRecords m_records[64];

	void UpdateRecord(int i);

	bool IsTickValid(int tick) const;


	int UpdateBestTarget(C_BasePlayer* pLocal, CUserCmd* pCmd);
};