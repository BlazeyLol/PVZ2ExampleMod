#include "NameMapper.hpp"

std::vector<std::string> g_modZombieTypenames;
std::vector<std::string> g_modPlantTypenames;

#define REGISTER_ZOMBIE_TYPENAME(typename) \
    g_modZombieTypenames.push_back(typename); \

#define REGISTER_PLANT_TYPENAME(typename) \
    g_modPlantTypenames.push_back(typename); \

typedef ZombieAlmanac* (*ZombieAlmanacCtor)(ZombieAlmanac*);
ZombieAlmanacCtor oZombieAlmanacCtor = NULL;

void* hkCreateZombieTypenameMap(ZombieAlmanac* a1)
{
    // Let the game create the original alias->id map
    oZombieAlmanacCtor(a1);
    // Now add our own zombie aliases to it 
    // (mod aliases can be registered with the REGISTER_ZOMBIE_TYPENAME macro)
    g_modZombieTypenames.clear();
    NameMapperBase::PreHookZombieNameMapper();

    LOGI("Extra zombie typenames = %d", g_modZombieTypenames.size());

    for (INT iter = 0; iter < g_modZombieTypenames.size(); iter++)
    {
        LOGI("Registering extra zombie typename %s", g_modZombieTypenames[iter].c_str());
        a1->m_aliasToId[g_modZombieTypenames[iter]] = FIRST_FREE_ZOMBIE_ID + iter;
    }

    return a1;
}

typedef PlantNameMapper* (*PlantNameMapperCtor)(PlantNameMapper*);
PlantNameMapperCtor oPlantNameMapperCtor = NULL;

void* hkCreatePlantNameMapper(PlantNameMapper* self)
{
    // Same deal with the ZombieAlmanac::ctor hook
    oPlantNameMapperCtor(self);

    g_modPlantTypenames.clear();
    NameMapperBase::PreHookPlantNameMapper();

    LOGI("Extra typenames size = %d", g_modPlantTypenames.size());

    for (INT iter = 0; iter < g_modPlantTypenames.size(); iter++)
    {
        LOGI("Registering plant %s", g_modPlantTypenames[iter].c_str());
        self->m_aliasToId[g_modPlantTypenames[iter]] = FIRST_FREE_PLANT_ID + iter;
    }

    return self;
}

inline void NameMapperBase::PreHookZombieNameMapper()
{
    REGISTER_ZOMBIE_TYPENAME("iceage_armor4");
}

inline void NameMapperBase::PreHookPlantNameMapper()
{
    REGISTER_PLANT_TYPENAME("tacticalcuke");
    REGISTER_PLANT_TYPENAME("icycurrant");
}

void NameMapperBase::NameMapperHooks()
{
    LOGI("Init Feature: Custom Typenames");
    PVZ2HookFunction(OFFSET_ZOMBIE_ALMANAC_CTOR, (void*)hkCreateZombieTypenameMap, (void**)&oZombieAlmanacCtor, "ZombieAlmanac::ZombieAlmanac");
    PVZ2HookFunction(OFFSET_PLANT_NAME_MAPPER_CTOR, (void*)hkCreatePlantNameMapper, (void**)&oPlantNameMapperCtor, "PlantNameMapper::PlantNameMapper");
}