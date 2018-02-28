#include "Windows.h"
#include <cstdio>
#include "valve_sdk/csgostructs.hpp"
#include "Utils.h"
#include "Hooks.h"

void Begin(LPVOID base)
{
	//AllocConsole();
	//AttachConsole(GetCurrentProcessId());
	//freopen(("CON"), ("w"), stdout);
	//SetConsoleTitle("MARKS FUCKING LAME");
	
	Utils::AttachConsole();

	Interfaces::Initialize();
	Interfaces::Dump();

	NetvarSys::Get().Initialize();

	Hooks::Initialize();

	while (!GetAsyncKeyState(VK_END))
	{
		Sleep(100);
	}
	Hooks::UnloadHooks(base);
}

DWORD WINAPI End()
{
	return true;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(Begin), hModule, 0, nullptr);
		return TRUE;
	case DLL_PROCESS_DETACH:
		if (lpReserved == nullptr)
			return End();
		return TRUE;
	default:
		return TRUE;
	}
}