#include "ESP.h"
#include "Hooks.h"
#include "Utils.h"

void ESP::BoxESP()
{
	for (auto i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (!entity || entity == g_LocalPlayer || entity->IsDormant() || !entity->IsAlive())
			continue;

		RECT drawrect = GetBBox(entity);

		bool is_friendly = g_LocalPlayer->m_iTeamNum() == entity->m_iTeamNum();

		Color colorVis = is_friendly ? Color(10, 10, 200, 255) : Color(200, 60, 60, 255);
		Color colorHid = is_friendly ? Color(10, 200, 200, 255) : Color(232, 209, 32, 255);

		DrawBox(*Hooks::renderer, entity, colorVis, colorHid, drawrect);
	}
}

bool ESP::GetBox(DrawManager & renderer, C_BasePlayer * entity, int & topOut, int & bottomOut, int & leftOut, int & rightOut)
{
	if (!entity)
		return false;

	const matrix3x4_t& trans = entity->m_rgflCoordinateFrame();
	if (!trans.Base())
		return false;

	Vector min;
	Vector max;

	Vector brt;
	Vector blt;
	Vector frt;
	Vector blb;
	Vector flt;
	Vector flb;
	Vector frb;
	Vector brb;

	ICollideable* collision = entity->GetCollideable();

	if (!collision)
		return false;

	min = collision->OBBMins();
	max = collision->OBBMaxs();

	Vector playerpoints[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};


	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++)
	{
		Math::VectorTransform(playerpoints[i], trans, pointsTransformed[i]);
	}

	if (!Math::WorldToScreenOld(pointsTransformed[3], flb) || !Math::WorldToScreenOld(pointsTransformed[5], brt)
		|| !Math::WorldToScreenOld(pointsTransformed[0], blb) || !Math::WorldToScreenOld(pointsTransformed[4], frt)
		|| !Math::WorldToScreenOld(pointsTransformed[2], frb) || !Math::WorldToScreenOld(pointsTransformed[1], brb)
		|| !Math::WorldToScreenOld(pointsTransformed[6], blt) || !Math::WorldToScreenOld(pointsTransformed[7], flt))
		return false;

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;        // left
	float top = flb.y;        // top
	float right = flb.x;    // right
	float bottom = flb.y;    // bottom

	for (int i = 1; i < 8; i++)
	{
		if (bottom > arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top < arr[i].y)
			top = arr[i].y;
		if (left > arr[i].x)
			left = arr[i].x;
	}

	// Width / height
	leftOut = (int)left;
	rightOut = (int)right;
	topOut = (int)top;
	bottomOut = (int)bottom;

	return true;
}

RECT ESP::GetBBox(C_BasePlayer* temp)
{
	RECT rect{};

	auto collideable = temp->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = temp->m_rgflCoordinateFrame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}

	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

void ESP::DrawBox(DrawManager& renderer, C_BasePlayer* temp_target, Color color_vis, Color color_hid, RECT drawrect)
{
	
	Color color = g_LocalPlayer->CanSeePlayer(temp_target, HITBOX_HEAD) ? color_vis : color_hid;

	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawOutlinedRect(drawrect.left, drawrect.bottom, drawrect.right, drawrect.top);

	g_VGuiSurface->DrawSetColor(Color::Black());
	g_VGuiSurface->DrawOutlinedRect(drawrect.left - 1, drawrect.bottom - 1, drawrect.right + 1, drawrect.top + 1);
	g_VGuiSurface->DrawOutlinedRect(drawrect.left + 1, drawrect.bottom + 1, drawrect.right - 1, drawrect.top - 1);

}
