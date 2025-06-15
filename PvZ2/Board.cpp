#include "Board.hpp"

INT gWidth = 0;
INT gHeight = 0;

#ifdef __arm__
#define LAWNAPP_ADDR 0x1D92A94
#define SEXYAPP_ADDR 0x1D9CA24

#elif __aarch64__
#define LAWNAPP_ADDR 0x2593760
#define SEXYAPP_ADDR 0x25A4618

#endif

inline UINT getLawnApp() {
    return *(UINT*)getActualOffset(LAWNAPP_ADDR);
}

UINT getSexyApp() {
    return *(UINT*)getActualOffset(SEXYAPP_ADDR);
}

AspectRatio GetAspectRatio()
{
    float ratio = (float)gWidth / (float)gHeight;
    if (ratio <= 1.4f)
    {
        return Letterbox;
    }
    else if (ratio >= 1.41f && ratio <= 1.85f)
    {
        return Widescreen;
    }
    else if (ratio >= 1.86f)
    {
        return Ultrawide;
    }
}

typedef void(*ReinitForSurfaceChange)(INT, INT, INT, INT, INT);
ReinitForSurfaceChange oRFSC = nullptr;

void hkReinitForSurfaceChange(INT thisptr, INT a2, INT width, INT height, INT a5)
{
    gWidth = width;
    gHeight = height;
    return oRFSC(thisptr, a2, width, height, a5);
}

typedef Board* (*boardCtor)(Board*);
boardCtor oBoardCtor = NULL;

Board* hkBoardCtor(Board* board)
{
    oBoardCtor(board);

    LOGI("Board constructor called");

    board->m_lawnRect.mX = 200; //200
    board->m_lawnRect.mY = 160; //160

    switch (GetAspectRatio())
    {
        case Letterbox:
        {
            LOGI("Aspect Ratio: Letterbox");
            board->m_lawnRect.mWidth = 576; //576
            board->m_lawnRect.mHeight = 500; //380
            break;
        }
        case Widescreen:
        {
            LOGI("Aspect Ratio: Widescreen");
            board->m_lawnRect.mWidth = 576; //576
            board->m_lawnRect.mHeight = 450; //380
            break;
        }
        case Ultrawide:
        {
            LOGI("Aspect Ratio: Ultrawide");
            board->m_lawnRect.mX = 450;
            board->m_lawnRect.mY = 160;
            board->m_lawnRect.mWidth = 576; //576
            board->m_lawnRect.mHeight = 525; //380
            break;
        }
    }

    return board;
}

void printBoard(INT board) {
    LOGI("Print board");
    LOGI("State at 0x174");
    for (INT row = 0; row < 5; ++row) {
        std::string s = "";
        for (INT col = 0; col < 9; ++col) {
            s += std::to_string(*(UINT*)(board + 0x174 + (col * 5 + row) * 4)) + " ";
        }
        LOGI(s.c_str());
    }

    LOGI("State at 0x228");
    for (INT row = 0; row < 5; ++row) {
        std::string s = "";
        for (INT col = 0; col < 9; ++col) {
            s += std::to_string((INT)(*(char*)(board + 0x228 + col * 5 + row))) + " ";
        }
        LOGI(s.c_str());
    }
}

void Board::BoardHooks()
{
    LOGI("Init Feature: Board Zoom");
    PVZ2HookFunction(OFFSET_REINIT_FOR_SURFACE_CHANGE, (void*)hkReinitForSurfaceChange, (void**)&oRFSC, "ReinitForSurfaceChanged");
    //PVZ2HookFunction(OFFSET_BOARD_CTOR, (void*)hkBoardCtor, (void**)&oBoardCtor, "Board::Board");
}