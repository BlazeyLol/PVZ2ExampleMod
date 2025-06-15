#include "ImmunityMgr.hpp"

const bool enableImmunities = true;

// we don't need to save a pointer to the original functions
UINT dispose;

void hkMagicianHealerConditionFunc(INT a1, INT condition)
{
    // Hardcoded immunity checks would go here
    return;
}

bool hkMagicianHealerImmuneToShrink(INT a1)
{
    return enableImmunities;
}

bool hkMagicianInitializeFamilyImmunities(INT a1, LONG a2)
{
    typedef bool(*zFamilyFunc)(INT);
    zFamilyFunc func = (zFamilyFunc)getActualOffset(Z_FAMILY_FUNC_ADDR); // function 93 in Zombie's vftable 
    return func(a1);
}

void ImmunityMgr::ImmunityRemovalHook()
{
    LOGI("Init Feature: Removed Immunities");
    PVZ2HookFunction(OFFSET_MAGICIAN_IMMUNE, (void*)hkMagicianHealerImmuneToShrink, (void**)&dispose, "ZombieCarnieMagician::IsImmuneToShrink");
    PVZ2HookFunction(OFFSET_HEALER_IMMUNE, (void*)hkMagicianHealerImmuneToShrink, (void**)&dispose, "ZombieRomanHealer::IsImmuneToShrink");
    //PVZ2HookFunction(OFFSET_MAGICIAN_CONDITION, (void*)hkMagicianHealerConditionFunc, (void**)&dispose, "ZombieCarnieMagician::ConditionFunc");
    PVZ2HookFunction(OFFSET_HEALER_CONDITION, (void*)hkMagicianHealerConditionFunc, (void**)&dispose, "ZombieRomanHealer::ConditionFunc");
    PVZ2HookFunction(OFFSET_HEALER_INITIALIZE_FAMILY_IMMUNITIES, (void*)hkMagicianInitializeFamilyImmunities, (void**)&dispose, "ZombieRomanHealer::InitializeFamilyImmunities");
}