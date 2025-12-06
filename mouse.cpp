#include "mouse.hpp"
#include "mem.hpp"
#include "ui.hpp"
#include <iostream>

using std::cout;

BOOL(__stdcall* GetCursorPosOrig)(LPPOINT lpPoint) = nullptr;
static BOOL __stdcall GetCursorPosHook(LPPOINT lpPoint) {
	if (win_shown || no_cursor_kill)
		return FALSE;
	return GetCursorPosOrig(lpPoint);
}

HCURSOR(__stdcall* SetCursorOrig)(HCURSOR hCursor) = nullptr;
static HCURSOR __stdcall SetCursorHook(HCURSOR hCursor) {
	if (win_shown && 0)
		return nullptr;
	return SetCursorOrig(hCursor);
}

static unsigned int __stdcall SetCursorYHook(void* param_1, int param_2, void *pshit)
{
	if (win_shown || no_cursor_kill)
		return 0;
	BOOL uVar1;
	tagPOINT local_c;
	GetCursorPosOrig(&local_c);
	uVar1 = SetCursorPos(param_2, local_c.y);
	return uVar1 & 0xffff0000;
}

static unsigned int __stdcall SetCursorXHook(void* param_1, int param_2, void *pshit)
{
	if (win_shown || no_cursor_kill)
		return 0;
	BOOL uVar1;
	tagPOINT local_c;
	GetCursorPosOrig(&local_c);
	uVar1 = SetCursorPos(local_c.x, param_2);
	return uVar1 & 0xffff0000;
}

static SHORT(__stdcall* GetKeyStateOrig)(int) = nullptr;
static SHORT __stdcall GetKeyStateHook(int key) {
    if (win_shown) {
        return 0;
    }
    auto ret = GetKeyStateOrig(key);
    return ret;
}

static SHORT(__stdcall* GetAsyncKeyStateOrig)(int) = nullptr;
static SHORT __stdcall GetAsyncKeyStateHook(int key) {
    if (win_shown) {
        return 0;
    }
    auto ret = GetKeyStateOrig(key);
    return ret;
}

void init_mouse() {
    hook(get_ptr("GetAsyncKeyState", "user32.dll"), GetAsyncKeyStateHook, &GetAsyncKeyStateOrig);
    hook(get_ptr("GetKeyState", "user32.dll"), GetKeyStateHook, &GetKeyStateOrig);
    hook(get_ptr("GetCursorPos", "user32.dll"), GetCursorPosHook, &GetCursorPosOrig);
	hook(get_ptr("SetCursor", "user32.dll"), SetCursorHook, &SetCursorOrig);
	hook(get_base("kcmouse.mfx") + 0x1103, SetCursorYHook);
	hook(get_base("kcmouse.mfx") + 0x1125, SetCursorXHook);
}
