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
extern HWND hwnd;

static int need_mouse = 0;

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
    if (need_mouse > 0) {
        if (1) {
            cout << "mouse event" << need_mouse << "\n";
			*lpMsg = {0};
            lpMsg->hwnd = ::hwnd;
            lpMsg->message = need_mouse == 2 ? WM_LBUTTONDOWN : WM_LBUTTONUP;
            lpMsg->wParam = 0x0000;
            lpMsg->lParam = 0;
		}
        need_mouse = 0;
        TranslateMessage(lpMsg);
        DispatchMessageA(lpMsg);
        return TRUE;
	}
	BOOL ret = PeekMessageAOrig(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	if (ret) {
		// Fuck you, MMF2!
            if (lpMsg->message == WM_MOUSEMOVE || lpMsg->message == WM_LBUTTONDOWN ||
                lpMsg->message == WM_LBUTTONUP /* || lpMsg->message == WM_KEYDOWN ||
                lpMsg->message == WM_KEYUP */
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
            if ((lpMsg->message == WM_KEYDOWN && LOWORD(lpMsg->lParam) == 1) ||
                lpMsg->message == WM_KEYUP) {
			static WPARAM last_key = 0;
			static ULONGLONG last_del = 0;
			auto cur_ticks = GetTickCount64();
			if (last_key == lpMsg->wParam && cur_ticks - last_del < 2) {
				last_del = cur_ticks;
			}
			else {
				last_key = lpMsg->wParam;
				last_del = cur_ticks;
				if (lpMsg->message == WM_KEYDOWN && (last_key == VK_INSERT || last_key == VK_TAB)) {
					// cout << "SHOW/HIDE! \n";
					win_shown = !win_shown;
                } else if (last_key == 'T') {
                    need_mouse = lpMsg->message == WM_KEYDOWN ? 1 : 2;
                    // cout << "mouse event created" << need_mouse << "\n";
				}
			}
		}
	}
    if (lpMsg->message == WM_LBUTTONDOWN || lpMsg->message == WM_LBUTTONUP) {
        lpMsg->wParam = 0;
        lpMsg->lParam = 0;
        // cout << "real mouse\n";
	}
	return ret || need_mouse > 0;
}

static int(__stdcall* MessageBoxAOrig)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) = nullptr;
static int __stdcall MessageBoxAHook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
	if (skip_msg_box)
		return IDYES;
	return MessageBoxAOrig(hWnd, lpText, lpCaption, uType);
}

int (__cdecl *_stricmpOrig)(const char* s1, const char* s2) = nullptr;
int __cdecl _stricmpHook(const char* s1, const char* s2) {
	// cout << "timeHook " << s1 << " " << s2 << " " << ret << "\n";
    if (god_mode && strcmp(s2, "Die") == 0)
        return 1;
    auto ret = _stricmpOrig(s1, s2);
	return ret;
}

void init_msg() {
    cout << "base: " << get_base() << "\n";
	hook(get_ptr("PeekMessageA", "user32.dll"), PeekMessageAHook, &PeekMessageAOrig);
	hook(get_ptr("MessageBoxA", "user32.dll"), MessageBoxAHook, &MessageBoxAOrig);
	// hook(get_base("clickteam-movement-controller.mfx") + 0x11ca, FUN1_Hook, &FUN1_Orig);
	hook(get_ptr("_stricmp", "msvcrt.dll"), _stricmpHook, &_stricmpOrig);
}