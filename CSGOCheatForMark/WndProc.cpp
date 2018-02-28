#include "WndProc.h"
#include "Hooks.h"
#include "Settings.h"
extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WindowProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		Hooks::PressedKeys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		Hooks::PressedKeys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
		Hooks::PressedKeys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		Hooks::PressedKeys[VK_RBUTTON] = false;
		break;
	case WM_MBUTTONDOWN:
		Hooks::PressedKeys[VK_MBUTTON] = true;
		break;
	case WM_MBUTTONUP:
		Hooks::PressedKeys[VK_MBUTTON] = false;
		break;
	case WM_XBUTTONDOWN:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			Hooks::PressedKeys[VK_XBUTTON1] = true;
		}
		else if (button == XBUTTON2)
		{
			Hooks::PressedKeys[VK_XBUTTON2] = true;
		}
		break;
	}
	case WM_XBUTTONUP:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			Hooks::PressedKeys[VK_XBUTTON1] = false;
		}
		else if (button == XBUTTON2)
		{
			Hooks::PressedKeys[VK_XBUTTON2] = false;
		}
		break;
	}
	case WM_KEYDOWN:
		Hooks::PressedKeys[wParam] = true;
		break;
	case WM_KEYUP:
		Hooks::PressedKeys[wParam] = false;
		break;
	default: break;
	}

	///* Custom key switches below */

	static bool isDownInsert = false;
	static bool isClickedInsert = false;

	if (Hooks::PressedKeys[VK_HOME])
	{
		isClickedInsert = false;
		isDownInsert = true;
	}
	else if (!Hooks::PressedKeys[VK_HOME] && isDownInsert)
	{
		isClickedInsert = true;
		isDownInsert = false;
	}
	else
	{
		isClickedInsert = false;
		isDownInsert = false;
	}

	if (isClickedInsert)
	{
		Settings::menuopen = !Settings::menuopen;

		static ConVar* cl_mouseenable = g_CVar->FindVar(encrypt("cl_mouseenable"));
		cl_mouseenable->SetValue(!Settings::menuopen);
	}

	if (Settings::menuopen && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam) && Hooks::Initialized && !Settings::input_shouldListen)
		return true;

	return CallWindowProc(Hooks::OldWindowProc, hWnd, uMsg, wParam, lParam);
}
