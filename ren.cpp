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

void init_ren() {
    // hook(0x414900, fn1Hook, fn1Orig);
    hook(get_ptr("CreateRunObject", "Perspective.mfx"), CreateRunObjectVPHook, &CreateRunObjectVPOrig);
    hook(get_ptr("HandleRunObject", "Perspective.mfx"), HandleRunObjectVPHook,
         &HandleRunObjectVPOrig);
    hook(get_ptr("DisplayRunObject", "Viewport.mfx"), DisplayRunObjectVPHook,
         &DisplayRunObjectVPOrig);
}
