#include "EventListener.h"

EventListener::EventListener()
{
	g_GameEvents->AddListener(this, encrypt("player_hurt"), false);
	g_GameEvents->AddListener(this, encrypt("player_spawned"), false);
	g_GameEvents->AddListener(this, encrypt("round_start"), false);
	g_GameEvents->AddListener(this, encrypt("bomb_planted"), false);
	g_GameEvents->AddListener(this, encrypt("bomb_defused"), false);
	g_GameEvents->AddListener(this, encrypt("bomb_exploded"), false);
}

EventListener::~EventListener()
{
	g_GameEvents->RemoveListener(this);
}

void EventListener::FireGameEvent(IGameEvent* event)
{
	//if (!strcmp(event->GetName(), encrypt("player_hurt")))
	//{
	//	const auto pVictim = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt(encrypt("userid")))));
	//	const auto pAttacker = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt(encrypt("attacker")))));
	//	const auto health = event->GetInt(encrypt("health"));
	//	const auto armor = event->GetInt(encrypt("armor"));
	//	const auto weapon = event->GetString(encrypt("weapon"));
	//	const auto dmg_health = event->GetInt(encrypt("dmg_health"));
	//	const auto dmg_armor = event->GetInt(encrypt("dmg_armor"));
	//	const auto hitgroup = event->GetInt(encrypt("hitgroup")); // 1 = hs 2 = upper torso 3 = lower torso 4 = left arm 5 = right arm 6 = left leg 7 = right leg
	//	Container::Instance().Resolve<DamageInformer>()->OnPlayerHurt(pVictim, pAttacker, health, armor, weapon, dmg_health, dmg_armor, hitgroup);
	//}
	//else if (!strcmp(event->GetName(), encrypt("player_spawned")))
	{
		const auto pSpawned = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt(encrypt("userid")))));
		const auto inrestart = event->GetBool(encrypt("inrestart"));
		if (g_LocalPlayer != pSpawned)
			return;

		BackTrack::Get().OnLocalPlayerSpawn(inrestart);

		/*Container::Instance().Resolve<C4Timer>()->OnLocalPlayerSpawn(inrestart);
		Container::Instance().Resolve<DamageInformer>()->OnLocalPlayerSpawn(inrestart);
		Container::Instance().Resolve<BackTrack>()->OnLocalPlayerSpawn(inrestart);*/
	}
	/*else if (!strcmp(event->GetName(), encrypt("round_start")))
	{
		const auto timelimit = event->GetInt(encrypt("timelimit"));
		const auto fraglimit = event->GetInt(encrypt("fraglimit"));
		const auto objective = event->GetString(encrypt("objective"));
		Container::Instance().Resolve<C4Timer>()->OnRoundStart(timelimit, fraglimit, objective);
	}
	else if (!strcmp(event->GetName(), encrypt("bomb_planted")))
	{
		const auto pPlanter = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))));
		const auto site = event->GetInt(encrypt("site"));
		Container::Instance().Resolve<C4Timer>()->OnBombPlanted(pPlanter, site);
	}
	else if (!strcmp(event->GetName(), encrypt("bomb_defused")))
	{
		const auto pPlanter = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))));
		const auto site = event->GetInt(encrypt("site"));
		Container::Instance().Resolve<C4Timer>()->OnBombDefused(pPlanter, site);
	}
	else if (!strcmp(event->GetName(), encrypt("bomb_exploded")))
	{
		const auto pPlanter = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))));
		const auto site = event->GetInt(encrypt("site"));
		Container::Instance().Resolve<C4Timer>()->OnBombExploded(pPlanter, site);
	}*/
}

int EventListener::GetEventDebugID()
{
	return 42;
}
