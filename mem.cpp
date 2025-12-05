#define _WIN32_LEAN_AND_MEAN
#include <cassert>
#include <Windows.h>
#include <mem.hpp>

size_t get_base(const char* obj_name) {
	auto ret = (size_t)GetModuleHandleA(obj_name);
	assert(ret != 0);
	return (size_t)ret;
}

void* get_ptr(const char* func_name, const char* obj_name) {
	auto obj = GetModuleHandleA(obj_name);
	assert(obj != nullptr);
	auto ptr = GetProcAddress(obj, func_name);
	assert(ptr != nullptr);
	return ptr;
}
