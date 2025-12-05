#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "mem.hpp"
#include "rng.hpp"
#include "ui.hpp"
#include <cstdlib>
#include <iostream>
#include <sys/timeb.h>

using std::cout;

static void(__cdecl* ftimeOrig)(struct timeb* time) = nullptr;
static void __cdecl ftimeHook(struct timeb* time) {
	ftimeOrig(time);
	if (new_rng_custom_seed) {
		time->millitm = (unsigned short)new_rng_seed;
	}
	cout << "ftime called:" << time->millitm << ", " << (time->millitm & 0xffff) << std::endl;
}

static int(__cdecl* randOrig)() = nullptr;
static int __cdecl randHook() {
	int ret;
	if (new_rng_custom) {
		ret = new_rng;
	}
	else
		ret = rand();
	cout << "rand() got " << ret << std::endl;
	return ret;
}

void init_rng() {
	// hook(get_ptr("_ftime", "msvcrt.dll"), ftimeHook, &ftimeOrig);
    hook(get_ptr("rand", "msvcrt.dll"), randHook, &randOrig);
}
