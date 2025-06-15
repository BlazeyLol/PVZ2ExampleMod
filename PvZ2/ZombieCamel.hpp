#pragma once
#include "../memUtils.hpp"

#ifdef __arm__
#define OFFSET_CAMEL_ZOMBIE_FUNC 0x789DC8
#define CAMEL_MINIGAME_MODULE_FUNC 0x78CFA0

#elif __aarch64__
#define OFFSET_CAMEL_ZOMBIE_FUNC 0xB18DC4
#define CAMEL_MINIGAME_MODULE_FUNC 0xB1BE04

#endif

class ZombieCamel
{
public:
	static void ZombieCamelHooks();
};