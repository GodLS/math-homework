#include "Hooks.h"
#include "EncryptStrings.h"
#include "Utils.h"
#include "ESP.h"
#include "WndProc.h"
#include "BHOP.h"
#include "EventListener.h"
#include "Container.h"
#include "imgui2/imgui.h"
#include "imgui2/DX9/imgui_impl_dx9.h"
#include "D3D.h"
#include "RageBotSean.h"

std::unique_ptr<vft>        Hooks::d3dDeviceHook = nullptr;
std::unique_ptr<vft>        Hooks::clientModeHook = nullptr;
std::unique_ptr<vft>        Hooks::clientHook = nullptr;
std::unique_ptr<vft>        Hooks::vguiPanelHook = nullptr;
std::unique_ptr<vft>        Hooks::eventManagerHook = nullptr;

Hooks::CreateMove			Hooks::originalCreateMove = nullptr;
Hooks::PaintTraverse		Hooks::originalPaintTraverse = nullptr;
Hooks::FrameStageNotify		Hooks::originalFrameStageNotify = nullptr;
Hooks::EndScene				Hooks::originalEndScene = nullptr;
Hooks::Reset				Hooks::originalReset = nullptr;

HWND						Hooks::WindowHandle = nullptr;
bool						Hooks::Initialized	= false;
std::unique_ptr<DrawManager>Hooks::renderer	= nullptr;
WNDPROC						Hooks::OldWindowProc = nullptr;

bool						Hooks::PressedKeys[256] = {};


void Hooks::Initialize()
{
	while (!(WindowHandle = FindWindowA(encrypt("Valve001"), nullptr)))
		Sleep(100);

	if (WindowHandle)
	{
		// For registering keypresses on menu
		OldWindowProc = (WNDPROC)SetWindowLongPtr(WindowHandle, GWLP_WNDPROC, (LONG_PTR)WindowProcHandler);
	}

	uint32_t dwDevice = **(uint32_t**)(Utils::GetPattern(encrypt("shaderapidx9.dll"), encrypt("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);
	if ((d3dDeviceHook = std::make_unique<vft>((PPDWORD)dwDevice, true)))
	{
		if (!((originalReset = d3dDeviceHook->Hook(16, hkReset))))
		{
			return;
		}

		if (!((originalEndScene = d3dDeviceHook->Hook(42, hkEndScene))))
		{
			return;
		}
	}

	if ((clientModeHook = std::make_unique<vft>(PPDWORD(g_ClientMode), true)))
	{
		if (!((originalCreateMove = clientModeHook->Hook(24, CreateMove(hkCreateMove)))))
		{
			return;
		}
	}

	if ((clientHook = std::make_unique<vft>(PPDWORD(g_CHLClient), true)))
	{
		if (!((originalFrameStageNotify = clientHook->Hook(36, FrameStageNotify(hkFrameStageNotify)))))
		{
			return;
		}
	}

	if ((vguiPanelHook = std::make_unique<vft>(PPDWORD(g_VGuiPanel), true)))
	{
		if (!((originalPaintTraverse = vguiPanelHook->Hook(41, PaintTraverse(hkPaintTraverse)))))
		{
			return;
		}
	}

	eventManagerHook = std::make_unique<vft>(PPDWORD(g_GameEvents), true);
	auto container = &Container::Instance();
	container->Register(std::make_shared<EventListener>());
	container->Register(std::make_shared<BackTrack>());
}


bool __stdcall Hooks::hkCreateMove(float sample_input_frametime, CUserCmd* pCmd)
{
	auto orig = clientModeHook->GetOriginal<CreateMove>(24);
	//orig(g_ClientMode, sample_input_frametime, pCmd);

	if (!pCmd->command_number)
		return orig(g_ClientMode, sample_input_frametime, pCmd);

	if (!g_EngineClient->IsInGame())
		return orig(g_ClientMode, sample_input_frametime, pCmd);

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return orig(g_ClientMode, sample_input_frametime, pCmd);

	BHOP::bhop(pCmd);
	//BackTrack::Get().CreateMove_Post(g_LocalPlayer, pCmd);
	RageBotSean::Get().run(pCmd);

	//if (!Math::Clamp(pCmd->viewangles))
	//	return false;

	return orig(g_ClientMode, sample_input_frametime, pCmd);
}

void Hooks::hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
{
	static auto panelId = vgui::VPANEL{ 0 };
	static auto oPaintTraverse = vguiPanelHook->GetOriginal<PaintTraverse>(41);

	oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

	if (!panelId)
	{
		const auto panelName = g_VGuiPanel->GetName(panel);
		if (panelName[0] == 'F' && panelName[5] == 'O' && panelName[11] == 'y')
		{
			panelId = panel;
		}
	}
	else if (panelId == panel)
	{
		if (!g_EngineClient->IsInGame() || g_EngineClient->IsTakingScreenshot())
			return;

		if (!g_LocalPlayer)
			return;

		ESP::BoxESP();
		if (bestBacktrackData != nullptr)
		{		
			Vector pos;
			Math::WorldToScreen(bestBacktrackData->aimTargetPos, pos);
			g_VGuiSurface->DrawSetColor(Color::Red());
			g_VGuiSurface->DrawOutlinedCircle(pos.x, pos.y, 1, 50);
		}
	}
}

void __stdcall Hooks::hkFrameStageNotify(ClientFrameStage_t curStage)
{
	originalFrameStageNotify(g_CHLClient, curStage);
	BackTrack::Get().FrameStageNotify_Post(curStage);
}

long __stdcall Hooks::hkEndScene(IDirect3DDevice9* device)
{
	auto oEndScene = d3dDeviceHook->GetOriginal<EndScene>(42);
	D3D::Get().EndScene(device);
	return oEndScene(device);
}

long Hooks::hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	auto oReset = d3dDeviceHook->GetOriginal<Reset>(16);

	ImGui_ImplDX9_NewFrame(); //Invalidate GUI resources
	renderer->OnLostDevice();

	auto hr = oReset(device, pPresentationParameters);

	renderer->OnResetDevice();
	ImGui_ImplDX9_CreateDeviceObjects(); //Recreate GUI resources

	return hr;
}

bool Hooks::UnloadHooks(LPVOID hModule)
{
	d3dDeviceHook->RestoreTable();
	clientModeHook->RestoreTable();
	vguiPanelHook->RestoreTable();
	eventManagerHook->RestoreTable();
	clientHook->RestoreTable();

	Utils::DetachConsole();
	//Restore the WindowProc
	SetWindowLongPtr(WindowHandle, GWLP_WNDPROC, (LONG_PTR)OldWindowProc);
	FreeLibraryAndExitThread((HMODULE)hModule, 0);

	return true;
}
