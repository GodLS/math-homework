#pragma once

#include "valve_sdk/sdk.hpp"

#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)

namespace Math
{
	float FASTSQRT(float x);
	float VectorDistance(Vector v1, Vector v2);
	void Normalize(Vector angle);
	void NormalizeAngles(QAngle& angles);
	void ClampAngles(QAngle& angles);
	void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up);
	void ClampAngles(Vector &angles);
	bool Clamp(Vector &angles);
	//void VectorAngles(const Vector& forward, Vector& angles);
	void VectorAngles(const Vector *forward, Vector *angles);
	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	void AngleVectors(const QAngle &angles, Vector& forward);
	bool WorldToScreen(const Vector& in, Vector& out);
	bool WorldToScreenOld(const Vector& origin, Vector& screen);
	bool ScreenTransformOld(const Vector& point, Vector& screen);
	Vector CalcAngle(Vector src, Vector dst);
	Vector CalcAngle2(Vector& src, Vector& dst);

}
