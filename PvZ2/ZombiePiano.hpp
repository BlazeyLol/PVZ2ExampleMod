#pragma once
#include "../memUtils.hpp"
#include <vector>

#ifdef __arm__
#define OFFSET_INIT_ZOMBIE_PIANO_LIST 0x885F80
#define INIT_PIANO_LIST_PTR 0x1D890F4
#elif __aarch64__
#define OFFSET_INIT_ZOMBIE_PIANO_LIST 0xC1D1FC
#define INIT_PIANO_LIST_PTR 0x2581BD0
#endif

class ZombiePiano
{
public:
	static void ZombiePianoHooks();
};