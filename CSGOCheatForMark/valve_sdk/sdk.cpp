#include "sdk.hpp"
#include "../Utils.h"
#include "../interface_manager.h"

IVEngineClient*       g_EngineClient   = nullptr;
IBaseClientDLL*       g_CHLClient      = nullptr;
IClientEntityList*    g_EntityList     = nullptr;
IVDebugOverlay*       g_DebugOverlay = nullptr;
ISurface*             g_VGuiSurface = nullptr;
CGlobalVarsBase*      g_GlobalVars     = nullptr;
IGameEventManager2*   g_GameEvents = nullptr;
IMoveHelper*          g_MoveHelper = nullptr;
IPanel*               g_VGuiPanel      = nullptr;
IClientMode*          g_ClientMode     = nullptr;
CInput*               g_Input          = nullptr;
IVModelInfoClient*    g_MdlInfo        = nullptr;
IVModelRender*        g_MdlRender      = nullptr;
CGlowObjectManager*   g_GlowObjManager = nullptr;
IViewRender*          g_ViewRender = nullptr;
IDirect3DDevice9*     g_D3DDevice9 = nullptr;
IEngineTrace*         g_EngineTrace = nullptr;
ICvar*                g_CVar		= nullptr;
IVRenderView*         g_RenderView     = nullptr;
IMaterialSystem*      g_MatSystem      = nullptr;
IMDLCache*            g_MdlCache       = nullptr;
IPrediction*          g_Prediction     = nullptr;
CGameMovement*        g_GameMovement   = nullptr;
IEngineSound*         g_EngineSound    = nullptr;
CClientState*         g_ClientState    = nullptr;
IPhysicsSurfaceProps* g_PhysSurface    = nullptr;
IInputSystem*		  g_InputSystem	   = nullptr;

C_LocalPlayer         g_LocalPlayer;

namespace Interfaces
{
    CreateInterfaceFn get_module_factory(HMODULE module)
    {
        return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(module, "CreateInterface"));
    }

    template<typename T>
    T* grab_interface(CreateInterfaceFn f, const char* szInterfaceVersion)
    {
        auto result = reinterpret_cast<T*>(f(szInterfaceVersion, nullptr));

        if(!result)
		{
            throw std::runtime_error(std::string("[grab_interface] Failed to GetOffset interface: ") + szInterfaceVersion);
        }

        return result;
    }

	void Dump()
	{
		// Ugly macros ugh
#define STRINGIFY_IMPL(s) #s
#define STRINGIFY(s)      STRINGIFY_IMPL(s)
#define PRINT_INTERFACE(name) Utils::ConsolePrint("%-20s: %p\n", STRINGIFY(name), name)

		Utils::ConsolePrint("Interfaces:\n");
		PRINT_INTERFACE(g_CHLClient);
		PRINT_INTERFACE(g_EntityList);
		PRINT_INTERFACE(g_Prediction);
		PRINT_INTERFACE(g_GameMovement);
		PRINT_INTERFACE(g_MdlCache);
		PRINT_INTERFACE(g_EngineClient);
		PRINT_INTERFACE(g_MdlInfo);
		PRINT_INTERFACE(g_MdlRender);
		PRINT_INTERFACE(g_RenderView);
		PRINT_INTERFACE(g_EngineTrace);
		PRINT_INTERFACE(g_DebugOverlay);
		PRINT_INTERFACE(g_GameEvents);
		PRINT_INTERFACE(g_EngineSound);
		PRINT_INTERFACE(g_MatSystem);
		PRINT_INTERFACE(g_CVar);
		PRINT_INTERFACE(g_VGuiPanel);
		PRINT_INTERFACE(g_VGuiSurface);
		PRINT_INTERFACE(g_PhysSurface);
		Utils::ConsolePrint("|-------------------------------------------\n");

	}

    void Initialize()
    {
		auto dataCacheFactory = get_module_factory(GetModuleHandleW(L"datacache.dll"));
        auto engineFactory    = get_module_factory(GetModuleHandleW(L"engine.dll"));
        auto valveStdFactory  = get_module_factory(GetModuleHandleW(L"vstdlib.dll"));
    	auto matSysFactory = get_module_factory(GetModuleHandleW(L"materialsystem.dll"));
        auto vguiFactory      = get_module_factory(GetModuleHandleW(L"vguimatsurface.dll"));
        auto vgui2Factory     = get_module_factory(GetModuleHandleW(L"vgui2.dll"));        
    	auto vphysicsFactory = get_module_factory(GetModuleHandleW(L"vphysics.dll"));
		auto clientFactory = get_module_factory(GetModuleHandleW(L"client.dll"));

		g_MdlCache = grab_interface<IMDLCache>(dataCacheFactory, "MDLCache004");
        g_GameMovement        = grab_interface<CGameMovement>       (clientFactory   , "GameMovement001");
		g_MatSystem = grab_interface<IMaterialSystem>(matSysFactory, "VMaterialSystem080");
		g_Prediction = grab_interface<IPrediction>(clientFactory, "VClientPrediction001");
		g_DebugOverlay = grab_interface<IVDebugOverlay>(engineFactory, "VDebugOverlay004");
		g_RenderView = grab_interface<IVRenderView>(engineFactory, "VEngineRenderView014");
        g_EngineClient        = grab_interface<IVEngineClient>      (engineFactory   , "VEngineClient014");
        //g_MdlInfo             = grab_interface<IVModelInfoClient>   (engineFactory   , "VModelInfoClient004");
		g_MdlInfo             = interface_manager::bf_interface<IVModelInfoClient>   ("engine.dll", "VModelInfoClient");
		g_EntityList = grab_interface<IClientEntityList>(clientFactory, "VClientEntityList003");
		g_CVar = grab_interface<ICvar>(valveStdFactory, "VEngineCvar007");
        g_MdlRender           = grab_interface<IVModelRender>       (engineFactory   , "VEngineModel016");
		g_GameEvents			= grab_interface<IGameEventManager2>(engineFactory, "GAMEEVENTSMANAGER002");
        g_EngineTrace         = grab_interface<IEngineTrace>        (engineFactory   , "EngineTraceClient004");       
    	g_CHLClient = grab_interface<IBaseClientDLL>(clientFactory, "VClient018");
		g_VGuiSurface = grab_interface<ISurface>(vguiFactory, "VGUI_Surface031");
        g_EngineSound         = grab_interface<IEngineSound>        (engineFactory   , "IEngineSoundClient003");
        g_VGuiPanel           = grab_interface<IPanel>              (vgui2Factory    , "VGUI_Panel009");
        g_PhysSurface         = grab_interface<IPhysicsSurfaceProps>(vphysicsFactory , "VPhysicsSurfaceProps001");
		g_InputSystem =		interface_manager::bf_interface<IInputSystem>("inputsystem.dll", "InputSystemVersion");

        g_GlobalVars      =  **(CGlobalVarsBase***)(Utils::GetPattern("client.dll", "A1 ? ? ? ? 5E 8B 40 10") + 1);
		g_MoveHelper = **(IMoveHelper***)(Utils::GetPattern("client.dll", "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01") + 2);
        g_Input           =             *(CInput**)(Utils::GetPattern("client.dll", "B9 ? ? ? ? 8B 40 38 FF D0 84 C0 0F 85") + 1);
		g_ViewRender = *(IViewRender**)(Utils::GetPattern("client.dll", "A1 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? FF 10") + 1);
        g_GlowObjManager  = *(CGlowObjectManager**)(Utils::GetPattern("client.dll", "0F 11 05 ? ? ? ? 83 C8 01") + 3);
		g_ClientMode = **reinterpret_cast<IClientMode***>((*(DWORD**)g_CHLClient)[10] + 0x5);
			/**(IClientMode**)(Utils::GetPattern("client.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? 5D") + 1);*/
        g_ClientState     =     **(CClientState***)(Utils::GetPattern("engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1);
		g_D3DDevice9 = **(IDirect3DDevice9***)(Utils::GetPattern("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
        g_LocalPlayer     = *(C_LocalPlayer*)(Utils::GetPattern("client.dll", "8B 0D ? ? ? ? 83 FF FF 74 07") + 2);
    }

}