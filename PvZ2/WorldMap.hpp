#pragma once
#include "../memUtils.hpp"

#ifdef __arm__
#define OFFSET_WORLDMAP_DO_MOVEMENT 0x0
#elif __aarch64__
#define OFFSET_WORLDMAP_DO_MOVEMENT 0x0
#endif

class WorldMap
{
public:
    float m_float1, m_float2, m_float3, m_float4;
    float m_x, m_y, m_xMinusX, m_yMinusY;
    bool m_notAtEdge;

    static void WorldMapHooks();
};