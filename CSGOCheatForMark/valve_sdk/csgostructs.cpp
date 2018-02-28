#include "csgostructs.hpp"
#include "../Utils.h"
#include "../Math.h"


bool C_BaseEntity::IsPlantedC4()
{
	return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
}
bool C_BaseEntity::IsWeapon()
{
    //index: 160
    //ref: "CNewParticleEffect::DrawModel"
    //sig: 8B 80 ? ? ? ? FF D0 84 C0 74 6F 8B 4D A4
    return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 160)(this);
}
bool C_BaseEntity::IsDefuseKit()
{
	return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
}


bool C_BaseEntity::IsPlayer()
{
	//index: 152
	//ref: "effects/nightvision"
	//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 152)(this);
}

bool C_BaseCombatWeapon::HasBullets()
{
	return !IsReloading() && m_iClip1() > 0;
}


CCSWeaponInfo* C_BaseCombatWeapon::GetCSWeaponData()
{
    static auto fnGetWpnData
        = reinterpret_cast<CCSWeaponInfo*(__thiscall*)(void*)>(
            Utils::GetPattern("client.dll", "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B")
            );
    return fnGetWpnData(this);
}

bool C_BaseCombatWeapon::IsKnife()
{
	return GetCSWeaponData()->WeaponType == WEAPONTYPE_KNIFE;
}


bool C_BaseCombatWeapon::CanFire()
{
	if (IsReloading() || m_iClip1() <= 0)
		return false;

	if (!g_LocalPlayer)
		return false;

	float flServerTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;

	return m_flNextPrimaryAttack() <= flServerTime;
}

bool C_BaseCombatWeapon::IsRifle()
{
	switch (GetCSWeaponData()->WeaponType)
	{
	case WEAPONTYPE_RIFLE:
	case WEAPONTYPE_SHOTGUN:
	case WEAPONTYPE_SUBMACHINEGUN:
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}




bool C_BaseCombatWeapon::IsPistol()
{
    return GetCSWeaponData()->WeaponType == WEAPONTYPE_PISTOL;
}

bool C_BaseCombatWeapon::IsSniper()
{
    return GetCSWeaponData()->WeaponType == WEAPONTYPE_SNIPER_RIFLE;
}

float C_BaseCombatWeapon::GetSpread()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 484)(this);
}

void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 485)(this);
}
bool C_BaseCombatWeapon::IsGrenade()
{
	return GetCSWeaponData()->WeaponType == WEAPONTYPE_GRENADE;
}
float C_BaseCombatWeapon::GetInaccuracy()
{
    return CallVFunction<float(__thiscall*)(void*)>(this, 483)(this);
}

CUserCmd*& C_BasePlayer::m_pCurrentCommand()
{
	static auto currentCommand = *(uint32_t*)(Utils::GetPattern("client.dll", "89 BE ? ? ? ? E8 ? ? ? ? 85 FF") + 2);
	return *(CUserCmd**)((uintptr_t)this + currentCommand);
}



player_info_t C_BasePlayer::GetPlayerInfo()
{
	player_info_t info;
	g_EngineClient->GetPlayerInfo(EntIndex(), &info);
	return info;
}

bool C_BasePlayer::CanAttack()
{
	C_BaseCombatWeapon* weapon = this->m_hActiveWeapon();
	if (weapon)
	{
		return weapon->CanFire();
	}
}

bool C_BasePlayer::IsValid(bool visibleCheck, bool ignoreTeam)
{
	if (!this->IsPlayer())
		return false;

	if (!this->IsAlive())
		return false;

	if (this->IsDormant())
		return false;

	if (!ignoreTeam)
		if (this->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			return false;

	if (visibleCheck)
		if (!g_LocalPlayer->CanSeePlayer(this, HITBOX_HEAD))
			return false;

	return true;
}

float C_BasePlayer::GetFov(Vector viewAngle, Vector aimAngle)
{
	Vector ang, aim;
	Utils::angle_vectors(viewAngle, aim);
	Utils::angle_vectors(aimAngle, ang);

	auto fov = RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));

	fov = fov + (Math::VectorDistance(g_LocalPlayer->m_vecOrigin(), this->m_vecOrigin()) * 0.002f);
	return fov;
}

bool C_BaseCombatWeapon::IsReloading()
{
	static auto inReload = *(uint32_t*)(Utils::GetPattern("client.dll", "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

Vector C_BasePlayer::GetBonePos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f)) {
		return boneMatrix[bone].at(3);
	}
	return Vector{};
}


bool C_BasePlayer::IsAlive()
{
    return m_lifeState() == LIFE_ALIVE;
}


Vector C_BasePlayer::GetEyePos()
{
	return m_vecOrigin() + m_vecViewOffset();
}
bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto start = GetEyePos();
	auto dir = (pos - start).Normalized();

	ray.Init(start, pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}
Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
    matrix3x4_t boneMatrix[MAXSTUDIOBONES];

    if(SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f))
	{
        auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());

    	if(studio_model)
		{
            auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
            if(hitbox)
			{
                auto
                    min = Vector{},
                    max = Vector{};

                Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
                Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

                return (min + max) / 2.0f;
            }
        }
    }
    return Vector{};
}
bool C_BasePlayer::HasC4()
{
	static auto fnHasC4
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::GetPattern("client.dll", "56 8B F1 85 F6 74 31")
			);

	return fnHasC4(this);
}
bool C_BasePlayer::GetHitboxPos(int hitbox, Vector &output)
{
    if(hitbox >= HITBOX_MAX)
        return false;

    const model_t *model = this->GetModel();
    if(!model)
        return false;

    studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(model);
    if(!studioHdr)
        return false;

    matrix3x4_t matrix[MAXSTUDIOBONES];
    if(!this->SetupBones(matrix, MAXSTUDIOBONES, 0x100, 0))
        return false;

    mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
    if(!studioBox)
        return false;

    Vector min, max;

    Math::VectorTransform(studioBox->bbmin, matrix[studioBox->bone], min);
    Math::VectorTransform(studioBox->bbmax, matrix[studioBox->bone], max);

    output = (min + max) * 0.5f;

    return true;
}


bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, int hitbox)
{
    CGameTrace tr;
    Ray_t ray;
    CTraceFilter filter;
    filter.pSkip = this;

    auto endpos = player->GetHitboxPos(hitbox);

    ray.Init(GetEyePos(), endpos);
    g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

    return tr.hit_entity == player || tr.fraction > 0.97f;
}



