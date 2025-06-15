#pragma once
#include "../memUtils.hpp"
#include <map>
#include <vector>

#ifdef __arm__
#define OFFSET_ZOMBIE_ALMANAC_CTOR 0x10643E0
#define OFFSET_PLANT_NAME_MAPPER_CTOR 0xDA5C58

#elif __aarch64__
#define OFFSET_ZOMBIE_ALMANAC_CTOR 0x14665C4
#define OFFSET_PLANT_NAME_MAPPER_CTOR 0x11797B4

#endif

class NameMapperBase
{
public:
	static void NameMapperHooks();
    inline static void PreHookZombieNameMapper();
    inline static void PreHookPlantNameMapper();
};

class ZombieAlmanac : public NameMapperBase
{
public:
    void* vftable;
    std::map<std::string, INT> m_aliasToId;
};

class PlantNameMapper : public NameMapperBase
{
public:
    void* vftable;
    std::map<std::string, INT> m_aliasToId;
};

// 441/188 as for 9.6
#define FIRST_FREE_ZOMBIE_ID 441
#define FIRST_FREE_PLANT_ID 188