#include "Math.h"
#include "Utils.h"

namespace Math
{
	float FASTSQRT(float x)
	{
		unsigned int i = *(unsigned int*)&x;

		i += 127 << 23;
		// approximation of square root
		i >>= 1;
		return *(float*)&i;
	}

	float VectorDistance(Vector v1, Vector v2)
	{
		return FASTSQRT(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
	}

	void Normalize(Vector angle)
	{
		while (angle.x > 89.0f) {
			angle.x -= 180.f;
		}
		while (angle.x < -89.0f) {
			angle.x += 180.f;
		}
		while (angle.y > 180.f) {
			angle.y -= 360.f;
		}
		while (angle.y < -180.f) {
			angle.y += 360.f;
		}
	}

	void NormalizeAngles(QAngle& angles)
	{
		for (auto i = 0; i < 3; i++) {
			while (angles[i] < -180.0f) angles[i] += 360.0f;
			while (angles[i] >  180.0f) angles[i] -= 360.0f;
		}
	}
	//--------------------------------------------------------------------------------
	void ClampAngles(QAngle& angles)
	{
		if (angles.pitch > 89.0f) angles.pitch = 89.0f;
		else if (angles.pitch < -89.0f) angles.pitch = -89.0f;

		if (angles.yaw > 180.0f) angles.yaw = 180.0f;
		else if (angles.yaw < -180.0f) angles.yaw = -180.0f;

		angles.roll = 0;
	}	
	//--------------------------------------------------------------------------------
	void ClampAngles(Vector &angles)
	{
		if (angles.y > 180.0f)
			angles.y = 180.0f;
		else if (angles.y < -180.0f)
			angles.y = -180.0f;

		if (angles.x > 89.0f)
			angles.x = 89.0f;
		else if (angles.x < -89.0f)
			angles.x = -89.0f;

		angles.z = 0;
	}
	//--------------------------------------------------------------------------------
	bool Clamp(Vector &angles)
	{
		Vector a = angles;
		Normalize(a);
		ClampAngles(a);

		if (isnan(a.x) || isinf(a.x) ||
			isnan(a.y) || isinf(a.y) ||
			isnan(a.z) || isinf(a.z)) {
			return false;
		}

		angles = a;
		return true;

	}
	//--------------------------------------------------------------------------------
	void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up)
	{
		float sr, sp, sy, cr, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
		DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

		forward.x = (cp * cy);
		forward.y = (cp * sy);
		forward.z = (-sp);
		right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
		right.z = (-1 * sr * cp);
		up.x = (cr * sp * cy + -sr*-sy);
		up.y = (cr * sp * sy + -sr*cy);
		up.z = (cr * cp);
	}
	//--------------------------------------------------------------------------------
	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		out.x = in1.Dot(in2.m_flMatVal[0]) + in2.m_flMatVal[0][3];
		out.y = in1.Dot(in2.m_flMatVal[1]) + in2.m_flMatVal[1][3];
		out.z = in1.Dot(in2.m_flMatVal[2]) + in2.m_flMatVal[2][3];
		/*out[0] = in1.Dot(in2[0]) + in2[0][3];
		out[1] = in1.Dot(in2[1]) + in2[1][3];
		out[2] = in1.Dot(in2[2]) + in2[2][3];*/
	}
	//--------------------------------------------------------------------------------
	void AngleVectors(const QAngle &angles, Vector& forward)
	{
		float	sp, sy, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

		forward.x = cp*cy;
		forward.y = cp*sy;
		forward.z = -sp;
	}
	//--------------------------------------------------------------------------------

	/*void VectorAngles(const Vector& forward, Vector& angles)
	{
		float	tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0) {
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}*/

	void VectorAngles(const Vector *forward, Vector *angles)
	{
		float	tmp, yaw, pitch;

		if (forward->y == 0.f && forward->x == 0.f)
		{
			yaw = 0.f;
			if (forward->z > 0.f)
			{
				pitch = 270.f;
			}
			else
			{
				pitch = 90.f;
			}
		}
		else
		{
			yaw = (atan2(forward->y, forward->x) * 180.f / M_PI);
			if (yaw < 0)
				yaw += 360.f;

			tmp = sqrt(forward->x * forward->x + forward->y * forward->y);
			pitch = (atan2(-forward->z, tmp) * 180.f / M_PI);
			if (pitch < 0.f)
				pitch += 360.f;
		}

		angles->x = pitch;
		angles->y = yaw;
		angles->z = 0.f;
	}
	//--------------------------------------------------------------------------------
	VMatrix& WorldToScreenMatrix()
	{
		/*static uint64_t dwFunction = Helpers::GetPattern(encrypt("engine.dll"), encrypt("A1 ? ? ? ? 83 F8 01 7E 11 69 C8"));
		static uint64_t dwVMatrixPtr = *(uint64_t*)(dwFunction + 0x11);

		uint64_t dwVMatrix = *(uint64_t*)dwVMatrixPtr;

		uint64_t dwResult = dwVMatrix + 2 * 528 - 68;

		return (VMatrix&)(*(uint64_t*)dwResult);*/\

		/*auto functionPtr = ((uint32_t*)(*(uint32_t**)g_EngineClient))[37];
		auto renderPtr = *reinterpret_cast<uint32_t*>(functionPtr + 0x1);
		auto viewMatrixPtr = *reinterpret_cast<uint32_t*>(renderPtr + 0xDC) + 2 * 528 - 68;

		return (VMatrix&)(*(DWORD*)viewMatrixPtr);*/

		static ptrdiff_t ptrViewMatrix;
		if (!ptrViewMatrix)
		{
			ptrViewMatrix = static_cast<ptrdiff_t>(Utils::GetPattern(encrypt("client.dll"), encrypt("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9")));
			ptrViewMatrix += 0x3;
			ptrViewMatrix = *reinterpret_cast<uintptr_t*>(ptrViewMatrix);
			ptrViewMatrix += 176;
		}

		VMatrix& w2sMatrix = *(VMatrix*)ptrViewMatrix;
		return w2sMatrix;
	}
	//--------------------------------------------------------------------------------

	bool ScreenTransformOld(const Vector& point, Vector& screen)
	{
		float w;
		const VMatrix& worldToScreen = /*g_EngineClient->*/WorldToScreenMatrix();

		screen.y = worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3];
		screen.x = worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3];
		w = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];
		screen.z = 0.0f;

		bool behind = false;

		if (w < 0.001f)
		{
			behind = true;
			screen.x *= 100000;
			screen.y *= 100000;
		}
		else
		{
			behind = false;
			float invw = 1.0f / w;
			screen.x *= invw;
			screen.y *= invw;
		}

		return behind;
	}


	Vector CalcAngle(Vector src, Vector dst)
	{
		Vector angles;
		Vector delta = src - dst;

		VectorAngles(&delta, &angles);

		Normalize(angles);

		return angles;
	}

	Vector CalcAngle2(Vector& src, Vector& dst)
	{
		Vector vAngle;
		Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
		double hyp = sqrt(delta.x*delta.x + delta.y*delta.y);

		vAngle.x = (float)(asinf((delta.z + 64.06f) / hyp) * 57.295779513082f);
		vAngle.y = (float)(atanf(delta.y / delta.x) * 57.295779513082f);
		vAngle.z = 0.0f;

		if (delta.x >= 0.0)
			vAngle.y += 180.0f;

		return vAngle;
	}

	//--------------------------------------------------------------------------------

	bool WorldToScreenOld(const Vector& origin, Vector& screen)
	{
		if (!ScreenTransformOld(origin, screen))
		{
			int ScreenWidth, ScreenHeight;
			g_EngineClient->GetScreenSize(ScreenWidth, ScreenHeight);
			float x = ScreenWidth / 2.f;
			float y = ScreenHeight / 2.f;
			x += 0.5f * screen.x * ScreenWidth + 0.5f;
			y -= 0.5f * screen.y * ScreenHeight + 0.5f;
			screen.x = x;
			screen.y = y;
			return true;
		}

		return false;
	}


	//--------------------------------------------------------------------------------
	static bool screen_transform(const Vector& in, Vector& out)
	{
		static auto& w2sMatrix = WorldToScreenMatrix();/*g_EngineClient->WorldToScreenMatrix();*/

		out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
		out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
		out.z = 0.0f;

		float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

		if (w < 0.001f) {
			out.x *= 100000;
			out.y *= 100000;
			return false;
		}

		out.x /= w;
		out.y /= w;

		return true;
	}
	//--------------------------------------------------------------------------------
	bool WorldToScreen(const Vector& in, Vector& out)
	{
		if (screen_transform(in, out)) {
			int w, h;
			g_EngineClient->GetScreenSize(w, h);

			out.x = (w / 2.0f) + (out.x * w) / 2.0f;
			out.y = (h / 2.0f) - (out.y * h) / 2.0f;

			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------
}
