#include "ZombieCamel.hpp"

// The original function is broken for some reason. We don't need it
UINT oCamelZombieFunc = NULL;

typedef void(*camelMinigameModuleFunc)(INT, INT, INT);
camelMinigameModuleFunc cmmFunc = (camelMinigameModuleFunc)getActualOffset(CAMEL_MINIGAME_MODULE_FUNC);

void hkCamelZombieFunc(INT a1, INT a2, INT a3)
{
    // Redirect call to some function in CamelMinigameModule
    // This fixes the crash when camels are rising from the ground
    cmmFunc(a1, a2, a3);
}

void ZombieCamel::ZombieCamelHooks()
{
    LOGI("Init Feature: Mummy Memory Fix");
    PVZ2HookFunction(OFFSET_CAMEL_ZOMBIE_FUNC, (void*)hkCamelZombieFunc, (void**)&oCamelZombieFunc, "CamelZombie::vftable_func_0xEC");
}