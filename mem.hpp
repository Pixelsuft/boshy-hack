#pragma once
#include <cstddef>
#include <minhook/include/MinHook.h>
#include <cassert>

extern HWND hwnd;

size_t get_base(const char* obj_name = nullptr);
void* get_ptr(const char* func_name, const char* obj_name = nullptr);

template<typename T>
T get_ptr_caller(const char* func_name, const char* obj_name = nullptr) {
	return reinterpret_cast<T>(get_ptr(func_name, obj_name));
}

template<typename A>
void hook(A pTarget, LPVOID pDetour) {
	assert(MH_CreateHook(reinterpret_cast<LPVOID>(pTarget), pDetour, nullptr) == MH_OK);
}

template<typename A, typename T>
void hook(A pTarget, LPVOID pDetour, T* ppOriginal) {
	assert(MH_CreateHook(reinterpret_cast<LPVOID>(pTarget), pDetour, reinterpret_cast<LPVOID*>(ppOriginal)) == MH_OK);
}
