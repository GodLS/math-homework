#pragma once
#include "Windows.h"
#include <cstdint>
#include <string>
#include <Psapi.h>
#include "EncryptStrings.h"
#include "valve_sdk/Math/Vector.hpp"
#include "valve_sdk/math/QAngle.hpp"
#include "valve_sdk/csgostructs.hpp"
#include "BackTrack.h"

#define INRANGE(x,a,b)  (x <= b && x >= a)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[1]) | getBits(x[0]) << 4)
#define M_PI 3.14159265358979323846
class Utils
{
public:
	static uint64_t GetPattern(std::string szModule, std::string szSignature);

	static void AttachConsole();
	static void DetachConsole();
	static bool ConsolePrint(const char* fmt, ...);
	static float DistancePointToLine(Vector point, Vector lineOrigin, Vector direction);
	static float GetFov(Vector viewAngle, Vector aimAngle, int player);
	static void angle_vectors(const Vector &angles, Vector& forward);
	static float get_fov(const Vector& viewAngles, const Vector& aimAngles, C_BasePlayer* player);
	static float GetFov(Vector viewAngle, Vector aimAngle, C_BasePlayer* entity);
};
