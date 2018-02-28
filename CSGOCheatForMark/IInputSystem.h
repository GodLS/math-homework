#pragma once
#include "Misc.h"

class IInputSystem
{
	template<typename FuncType>
	inline FuncType callvfunc(void* ppClass, unsigned int index) {
		unsigned int* pVTable = *(unsigned int**)ppClass;
		unsigned int dwAddress = pVTable[index];
		return (FuncType)(dwAddress);
	}

public:
	void EnableInput(bool bEnable)
	{
		typedef void(__thiscall* OriginalFn)(void*, bool);
		return callvfunc<OriginalFn>(this, 11)(this, bEnable);
	}

	void ResetInputState()
	{
		typedef void(__thiscall* OriginalFn)(void*);
		return callvfunc<OriginalFn>(this, 39)(this);
	}

	bool IsButtonDown(ButtonCode_t code)
	{
		typedef bool(__thiscall* OriginalFn)(void*, ButtonCode_t);
		return callvfunc<OriginalFn>(this, 15)(this, code);
	}

	void GetCursorPosition(int* m_pX, int* m_pY)
	{
		typedef void(__thiscall* OriginalFn)(void*, int*, int*);
		return callvfunc<OriginalFn>(this, 56)(this, m_pX, m_pY);
	}

	const char* ButtonCodeToString(ButtonCode_t ButtonCode)
	{
		typedef const char*(__thiscall* OriginalFn)(void*, ButtonCode_t);
		return callvfunc<OriginalFn>(this, 40)(this, ButtonCode);
	}

	ButtonCode_t VirtualKeyToButtonCode(int nVirtualKey)
	{
		typedef ButtonCode_t(__thiscall* OriginalFn)(void*, int);
		return callvfunc<OriginalFn>(this, 44)(this, nVirtualKey);
	}

	int ButtonCodeToVirtualKey(ButtonCode_t code)
	{
		typedef int(__thiscall* OriginalFn)(void*, ButtonCode_t);
		return callvfunc<OriginalFn>(this, 45)(this, code);
	}
};
