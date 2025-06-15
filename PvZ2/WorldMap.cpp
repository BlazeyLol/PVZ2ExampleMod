#include "WorldMap.hpp"

// Here is the simple feature switch
// It doesnt work for ARM64 currently
const bool enableVerticalScrolling = true;

typedef int (*worldMapDoMovement)(WorldMap*, float, float, bool);
worldMapDoMovement oWorldMapDoMovement = NULL;

int hkWorldMapDoMovement(WorldMap* self, float fX, float fY, bool allowVerticalMovement)
{
    LOGI("[WorldMap::DoMovement] %.02f %.02f - %d", fX, fY, allowVerticalMovement);
    LOGI("%.02f %.02f %.02f %.02f", self->m_float1, self->m_float2, self->m_float3, self->m_float4);
    return oWorldMapDoMovement(self, fX, fY, enableVerticalScrolling);
}

void WorldMap::WorldMapHooks()
{
    LOGI("Init Feature: Vertical Map Scrolling");
    // Worldmap is inlined in ARM64
    // TODO: mess up with 0x7AB6FC
#ifdef __arm__
    PVZ2HookFunction(0x441068, (void*)hkWorldMapDoMovement, (void**)&oWorldMapDoMovement, "WorldMap::doMovement");
#endif
}