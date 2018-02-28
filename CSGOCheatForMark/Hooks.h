#pragma once
#include "Windows.h"
#include <memory>
#include "vft.h"
#include "valve_sdk/csgostructs.hpp"
#include <d3d9.h>
#include "DrawManager.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class Hooks : public Singleton<Hooks>
{
public:
	static HWND WindowHandle;
	static WNDPROC OldWindowProc;
	static bool PressedKeys[256];
	static bool Initialized;
	static std::unique_ptr<DrawManager> renderer;

	static std::unique_ptr<vft>       d3dDeviceHook;
	static std::unique_ptr<vft>       clientModeHook;
	static std::unique_ptr<vft>       clientHook;
	static std::unique_ptr<vft>		  vguiPanelHook;
	static std::unique_ptr<vft>		  eventManagerHook;

	using CreateMove = bool(__thiscall*)(IClientMode*, float, CUserCmd*);
	using PaintTraverse = void(__thiscall*)(IPanel*, vgui::VPANEL, bool, bool);
	using FrameStageNotify = void(__thiscall*)(IBaseClientDLL*, ClientFrameStage_t);
	using EndScene = long(__stdcall *)(IDirect3DDevice9*);
	using Reset = long(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

	static CreateMove originalCreateMove;
	static PaintTraverse originalPaintTraverse;
	static FrameStageNotify originalFrameStageNotify;
	static EndScene originalEndScene;
	static Reset originalReset;

	static void Initialize();


	static bool __stdcall hkCreateMove(float sample_input_frametime, CUserCmd* pCmd);
	static void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce);
	static void __stdcall hkFrameStageNotify(ClientFrameStage_t curStage);
	static long __stdcall hkEndScene(IDirect3DDevice9* device);
	static long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);

	static bool UnloadHooks(LPVOID hModule);
};
