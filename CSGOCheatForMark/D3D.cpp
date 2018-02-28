#include "D3D.h"
#include "Settings.h"
#include "EventListener.h"
#include "Hooks.h"
#include "imgui2/imgui.h"
#include "imgui2/DX9/imgui_impl_dx9.h"
#include "Menu.h"

void D3D::EndScene(IDirect3DDevice9* device)
{
	auto oEndScene = Hooks::Get().d3dDeviceHook->GetOriginal<Hooks::EndScene>(42);

	if (!Hooks::Initialized)
	{
		Hooks::Get().renderer = std::make_unique<DrawManager>(device);
		ImGui_ImplDX9_Init(Hooks::Get().WindowHandle, device);
		Hooks::Get().Initialized = true;

	}
	ImGui_ImplDX9_NewFrame();

	if (Settings::menuopen)
	{
		DrawMenu();
	}
	Hooks::renderer->BeginRendering();

	//if (!g_EngineClient->IsInGame() || g_EngineClient->IsTakingScreenshot())
	//	return;

	ImGui::Render();
	Hooks::Get().renderer->EndRendering();

}
