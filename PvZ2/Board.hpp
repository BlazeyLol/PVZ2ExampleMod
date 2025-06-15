#pragma once
#include "../memUtils.hpp"

#define GRID_TILE_WIDTH 64.0f
#define GRID_TILE_HEIGHT 76.0f
#define GRID_TILE_TOP 160.0f
#define GRID_TILE_LEFT 200.0f

#define BOARD_START_POS 232.0f
#define BOARD_END_POS 776.0f

#ifdef __arm__
#define OFFSET_REINIT_FOR_SURFACE_CHANGE 0x12873BC
#define OFFSET_BOARD_CTOR 0x70F63C

#elif __aarch64__
#define OFFSET_REINIT_FOR_SURFACE_CHANGE 0x16752B4
#define OFFSET_BOARD_CTOR 0xA99D24

#endif

class Board
{
public:
	char pad_0000[148]; //0x0000
	INT m_columnCount; //0x0094
	INT m_rowCount; //0x0098
	char pad_009C[676]; //0x009C
	Rect m_lawnRect; //0x0340

	static void BoardHooks();
};

enum AspectRatio
{
	Letterbox,
	Widescreen,
	Ultrawide,
};

UINT getLawnApp();
UINT getSexyApp();
void printBoard(INT board);