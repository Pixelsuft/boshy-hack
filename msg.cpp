#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <imgui/backends/imgui_impl_win32.h>
#include "msg.hpp"
#include "mem.hpp"
#include "ui.hpp"
#include <cstdlib>
#include <iostream>

using std::cout;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static BOOL(__stdcall* PeekMessageAOrig)(
	LPMSG lpMsg,
	HWND  hWnd,
	UINT  wMsgFilterMin,
	UINT  wMsgFilterMax,
	UINT  wRemoveMsg
	) = nullptr;
static BOOL __stdcall PeekMessageAHook(
	LPMSG lpMsg,
	HWND  hWnd,
	UINT  wMsgFilterMin,
	UINT  wMsgFilterMax,
	UINT  wRemoveMsg
) {
	BOOL ret = PeekMessageAOrig(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	if (ret) {
		// Fuck you, MMF2!
		if (lpMsg->message == WM_MOUSEMOVE || lpMsg->message == WM_LBUTTONDOWN || lpMsg->message == WM_LBUTTONUP || lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_KEYUP 
			|| lpMsg->message == WM_CHAR) {
			static bool should_peak = true;
			if (!should_peak) {
				should_peak = true;
			}
			else {
				should_peak = false;
				// hkWindowProc(hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
				TranslateMessage(lpMsg);
				DispatchMessageA(lpMsg);
				ImGui_ImplWin32_WndProcHandler(hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
				//cout << "WTF";
			}
		}
		if (lpMsg->message == WM_KEYDOWN && LOWORD(lpMsg->lParam) == 1) {
			static WPARAM last_key = 0;
			static ULONGLONG last_del = 0;
			auto cur_ticks = GetTickCount64();
			if (last_key == lpMsg->wParam && cur_ticks - last_del < 2) {
				last_del = cur_ticks;
			}
			else {
				last_key = lpMsg->wParam;
				last_del = cur_ticks;
				if (last_key == VK_INSERT || last_key == VK_TAB) {
					// cout << "SHOW/HIDE! \n";
					win_shown = !win_shown;
				}
			}
		}
	}
	return ret;
}

static int(__stdcall* MessageBoxAOrig)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) = nullptr;
static int __stdcall MessageBoxAHook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
	if (skip_msg_box)
		return IDYES;
	return MessageBoxAOrig(hWnd, lpText, lpCaption, uType);
}

int (__cdecl *_stricmpOrig)(const char* s1, const char* s2) = nullptr;
int __cdecl _stricmpHook(const char* s1, const char* s2) {
	auto ret = _stricmpOrig(s1, s2);
	// cout << "timeHook " << s1 << " " << s2 << " " << ret << "\n";
	if (god_mode && strcmp(s2, "Die") == 0)
		return 1;
	return ret;
}

int(__cdecl* FUN1_Orig)(void* param_1, int param_2) = nullptr;
int __cdecl FUN1_Hook(void* param_1, int param_2) {
	cout << "FUN HOOK" << std::endl;
	return FUN1_Orig(param_1, param_2);
}

void init_msg() {
	hook(get_ptr("PeekMessageA", "user32.dll"), PeekMessageAHook, &PeekMessageAOrig);
	hook(get_ptr("MessageBoxA", "user32.dll"), MessageBoxAHook, &MessageBoxAOrig);
	// hook(get_base("clickteam-movement-controller.mfx") + 0x11ca, FUN1_Hook, &FUN1_Orig);
	hook(get_ptr("_stricmp", "msvcrt.dll"), _stricmpHook, &_stricmpOrig);
}