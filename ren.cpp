#include "rng.hpp"
#include "mem.hpp"
#include "ui.hpp"
#include <cstdlib>
#include <iostream>

using std::cout;

static void*(__stdcall* CreateTransitionOrig)(void* param_1, void* param_2,
                                              void* param_3) = nullptr;
static void* __stdcall CreateTransitionHook(void* param_1, void* param_2, void* param_3) {
    auto ret = CreateTransitionOrig(param_1, param_2, param_3);
    return ret;
}

static void*(__stdcall* CreateRunObjectVPOrig)(void* param_1, void* param_2,
                                               void* param_3) = nullptr;
static void* __stdcall CreateRunObjectVPHook(void* param_1, void* param_2, void* param_3) {
    cout << "HOOK CREATE\n";
    return nullptr;
    auto ret = CreateRunObjectVPOrig(param_1, param_2, param_3);
    return ret;
}

static void*(__stdcall* HandleRunObjectVPOrig)(void* param_1) = nullptr;
static void* __stdcall HandleRunObjectVPHook(void* param_1) {
    cout << "HOOK RUN\n";
    return nullptr;
    auto ret = HandleRunObjectVPOrig(param_1);
    return ret;
}

static short (__stdcall *DisplayRunObjectVPOrig)(void* pthis) = nullptr;
static short __stdcall DisplayRunObjectVPHook(void* pthis) {
    if (no_viewport)
        return 0;
    auto ret = DisplayRunObjectVPOrig(pthis);
    return ret;
}

static int(__cdecl* GetTValue)(uint8_t* param_1);
static uint8_t*(__cdecl* GetTargetObjPointer)(uint8_t* param_1);
static void(__cdecl* xPosOrig)(uint8_t* pthis) = nullptr;
static void __cdecl xPosHook(uint8_t* _pthis) {
    auto pthis = GetTargetObjPointer(_pthis);
    if (pthis == nullptr)
        return;
    int val = GetTValue(_pthis + 0xe);
    if (*(int*)(pthis + 0xcc) == 5) {
        // cout << "IDK IF ITS USED\n";
        xPosOrig(_pthis);
        return;
    }
    if (*(int*)(pthis + 0xcc) == 0xe) {
        xPosOrig(_pthis);
        return;
    }
    int* ptr = (int*)(pthis + 0x4c);
    if (*ptr == val) {
        xPosOrig(_pthis);
        return;
    }
    // TODO: fully reimpl.
    if (val > 150 && val < 250) {
        // 1c000080
        // 0b000080
        // 0c000080
        // 0d000080
        // 0e000080
        // 10000000
        // 13000000
        // 11000080
        // 12000080
        // 0F000000
        // cout << std::hex << (size_t)pthis << " " << val << " " << *(short*)pthis << "\n";
    }
    int idx = *(int*)pthis;
    short i = *(short*)pthis;
    if (i == 0x1c) {
        ppos[0] = val;
        cout << "fucked pos " << std::hex << (size_t)pthis;
        // return;
    }
    *ptr = val;
}

void init_ren() {
    GetTValue = reinterpret_cast<decltype(GetTValue)>(0x414900);
    GetTargetObjPointer = reinterpret_cast<decltype(GetTargetObjPointer)>(0x449500);
    // hook(0x411980, xPosHook, &xPosOrig);
    //hook(get_ptr("CreateRunObject", "Perspective.mfx"), CreateRunObjectVPHook, &CreateRunObjectVPOrig);
    //hook(get_ptr("HandleRunObject", "Perspective.mfx"), HandleRunObjectVPHook,
     //    &HandleRunObjectVPOrig);
    hook(get_ptr("DisplayRunObject", "Viewport.mfx"), DisplayRunObjectVPHook,
         &DisplayRunObjectVPOrig);
}
