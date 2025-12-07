#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d9.h>
#include <minhook/include/MinHook.h>
#include <kiero/kiero.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx9.h>
#include <cstdio>
#include <iostream>
#include "ui.hpp"
#include "hooks.hpp"

typedef long(__stdcall* Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
static Reset oReset = nullptr;

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
static EndScene oEndScene = nullptr;

HWND hwnd = nullptr;

using std::cout;
using std::endl;

static long __stdcall hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	long result = oReset(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}

WNDPROC oWndProc = NULL;
LRESULT CALLBACK hkWindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam,
                              _In_ LPARAM lParam) {
    return ::CallWindowProcA(oWndProc, hwnd, uMsg, wParam, lParam);
}


static long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	static bool init = false;

	if (!init)
	{
		D3DDEVICE_CREATION_PARAMETERS params;
		pDevice->GetCreationParameters(&params);
		hwnd = params.hFocusWindow;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX9_Init(pDevice);
        // oWndProc = (WNDPROC)::SetWindowLongPtr((HWND)hwnd, GWLP_WNDPROC, (LONG)hkWindowProc);
		init = true;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ui_render();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return oEndScene(pDevice);
}

static void impl_d3d9_init()
{
	hwnd = FindWindowA(nullptr, "I Wanna Be The Boshy");
	std::cout << "hwnd: " << hwnd << '\n';
	std::cout << "init: " << kiero::init(kiero::RenderType::Auto) << '\n';
	std::cout << "hkReset: " << kiero::bind(16, (void**)&oReset, hkReset) << '\n';
	std::cout << "hkEndScene: " << kiero::bind(42, (void**)&oEndScene, hkEndScene) << '\n';
}

static int MyThread()
{
#if defined(_DEBUG)
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
#endif
	impl_d3d9_init();
	init_hooks();
	MH_EnableHook(MH_ALL_HOOKS);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    DisableThreadLibraryCalls(hModule);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MyThread, nullptr, 0, nullptr);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

