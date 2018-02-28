#pragma once
#include "valve_sdk/Math/Vector.hpp"
#include "Math.h"
#include "../CSGOCheatForMark/valve_sdk/csgostructs.hpp"
#include "DrawManager.h"

class ESP
{
public:
	static void BoxESP();
private:
	static bool GetBox(DrawManager& renderer, C_BasePlayer* entity, int& topOut, int& bottomOut, int& leftOut, int& rightOut);
	static void DrawBox(DrawManager& renderer, C_BasePlayer* temp_target, Color color_vis, Color color_hid, RECT drawrect);
	static RECT GetBBox(C_BasePlayer* temp);
};
