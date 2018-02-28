#include "RageBotSean.h"
#include "Utils.h"
#include "Settings.h"

#include "Hooks.h"

void RageBotSean::run(CUserCmd* pCmd)
{
	//if (!g_InputSystem->IsButtonDown(static_cast<ButtonCode_t>(Settings::legit_aimkey)) && !g_InputSystem->IsButtonDown(static_cast<ButtonCode_t>(Settings::legit_aimkey2)))
		//return;

	if (!Hooks::PressedKeys[Settings::legit_aimkey] && !Hooks::PressedKeys[Settings::legit_aimkey2])
		return;

	C_BasePlayer* target = nullptr;

	if (target == nullptr || !target->IsValid(true, false))
		target = get_target(pCmd);

	if (target != nullptr && target->IsValid(true, false))
		aim_target(target, pCmd);
}

C_BasePlayer* RageBotSean::get_target(CUserCmd* pCmd)
{
	C_BasePlayer* result = nullptr;
	float best_fov = 180.f;
	for (int i = 0; i < g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer* entity = C_BasePlayer::GetPlayerByIndex(i);
		if (!entity || !entity->IsValid(true, false))
			continue;

		const auto calc_angle = Math::CalcAngle(g_LocalPlayer->GetEyePos(), entity->GetHitboxPos(HITBOX_HEAD));

		Vector view_angles;
		g_EngineClient->GetViewAngles(view_angles);
		view_angles += g_LocalPlayer->m_aimPunchAngle() * 2.f;

		float fov = Utils::get_fov(view_angles, calc_angle, entity);
		if (fov < 0)
			continue;

		if (fov < best_fov || fov > 180)
		{
			best_fov = fov;
			result = entity;			
		}
	}
	return result;

}

void RageBotSean::aim_target(C_BasePlayer* target, CUserCmd* pCmd)
{
	Vector eye_position = g_LocalPlayer->GetEyePos();
	Vector enemy_aimspot = target->GetHitboxPos(HITBOX_HEAD);

	Vector aimbot_angle = Math::CalcAngle2(eye_position,enemy_aimspot);
	aimbot_angle -= g_LocalPlayer->m_aimPunchAngle() * 2;

	if (!Math::Clamp(aimbot_angle))
		return;

	pCmd->viewangles = aimbot_angle;
}
