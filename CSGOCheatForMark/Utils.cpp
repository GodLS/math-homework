#include "Utils.h"
#include <iostream>

HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

uint64_t Utils::GetPattern(std::string szModule, std::string szSignature)
{
	const char* pattern = szSignature.c_str();
	DWORD firstSimilar = 0;
	DWORD startPoint;
	startPoint = DWORD(GetModuleHandleA(szModule.c_str()));
	MODULEINFO info;
	GetModuleInformation(GetCurrentProcess(), HMODULE(startPoint), &info, sizeof(MODULEINFO));
	DWORD endPoint;
	endPoint = info.SizeOfImage + startPoint;
	for (DWORD current = startPoint; current < endPoint; current++)
	{
		if (!*pattern)
		{
			return firstSimilar;
		}
		if (*PBYTE(pattern) == encrypt('\?') || *(BYTE*)current == getByte(pattern))
		{
			if (!firstSimilar)
			{
				firstSimilar = current;
			}

			if (!pattern[2])
			{
				return firstSimilar;
			}

			if (*PBYTE(pattern) != encrypt('\?') || *PWORD(pattern) == encrypt('\?\ ? '))
			{
				pattern += 3;
			}
			else
			{
				pattern += 2;
			}
		}
		else
		{
			pattern = szSignature.c_str();
			firstSimilar = 0;
		}
	}
	return NULL;
}

void Utils::AttachConsole()
{
	_old_out = GetStdHandle(STD_OUTPUT_HANDLE);
	_old_err = GetStdHandle(STD_ERROR_HANDLE);
	_old_in = GetStdHandle(STD_INPUT_HANDLE);

	::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

	_out = GetStdHandle(STD_OUTPUT_HANDLE);
	_err = GetStdHandle(STD_ERROR_HANDLE);
	_in = GetStdHandle(STD_INPUT_HANDLE);

	SetConsoleMode(_out,
	               ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

	SetConsoleMode(_in,
	               ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
	               ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
}

void Utils::DetachConsole()
{
	if (_out && _err && _in) {
		FreeConsole();

		if (_old_out)
			SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
		if (_old_err)
			SetStdHandle(STD_ERROR_HANDLE, _old_err);
		if (_old_in)
			SetStdHandle(STD_INPUT_HANDLE, _old_in);
	}
}

bool Utils::ConsolePrint(const char* fmt, ...)
{
	if (!_out)
		return false;

	char buf[1024];
	va_list va;

	va_start(va, fmt);
	_vsnprintf_s(buf, 1024, fmt, va);
	va_end(va);

	return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
}

float Utils::DistancePointToLine(Vector point, Vector lineOrigin, Vector direction)
{
	auto pointDir = point - lineOrigin;
	const auto tempOffset = pointDir.Dot(direction) / (direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
	if (tempOffset < 0.000001f)
		return FLT_MAX;

	const auto perpendicularPoint = lineOrigin + direction * tempOffset;
	return (point - perpendicularPoint).Length();
}

float Utils::GetFov(Vector viewAngle, Vector aimAngle, int count)
{
	auto player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(count));

	Vector delta = aimAngle - viewAngle;
	Math::Normalize(delta);

	float fov = sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));

	fov = fov + (Math::VectorDistance(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin()) * 0.002f);

	return fov;
}

void Utils::angle_vectors(const Vector &angles, Vector& forward)
{
	float	sp, sy, cp, cy;

	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

	forward.x = cp*cy;
	forward.y = cp*sy;
	forward.z = -sp;
}

float Utils::get_fov(const Vector &viewAngles, const Vector &aimAngles, C_BasePlayer* player)
{
	Vector ang, aim;
	angle_vectors(viewAngles, aim);
	angle_vectors(aimAngles, ang);

	auto fov = RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));

	fov = fov + (Math::VectorDistance(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin()) * 0.002f);
	return fov;
}

float Utils::GetFov(Vector viewAngle, Vector aimAngle, C_BasePlayer* entity)
{
	Vector delta = aimAngle - viewAngle;
	Math::Normalize(delta);

	float fov = sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));

	fov = fov + (Math::VectorDistance(g_LocalPlayer->m_vecOrigin(), entity->m_vecOrigin()) * 0.002f);

	return fov;
}

