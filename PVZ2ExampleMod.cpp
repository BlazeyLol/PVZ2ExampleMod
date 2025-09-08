#include "PVZ2ExampleMod.hpp"

#include <map>
#include <vector>

#include "Logging.hpp"
#include "memUtils.hpp"

#include "PvZ2/Board.hpp"
#include "PvZ2/WorldMap.hpp"

const char* hook::getPlatformABI()
{
#ifdef __arm__
    return "armeabi-v7a";
#elif __aarch64__
    return "arm64-v8a";
#endif
}

#pragma region Alias to ID

#ifdef __arm__
#define OFFSET_ZOMBIE_ALMANAC_CTOR 0x10643E0
#define OFFSET_PLANT_NAME_MAPPER_CTOR 0xDA5C58

#elif __aarch64__
#define OFFSET_ZOMBIE_ALMANAC_CTOR 0x14665C4
#define OFFSET_PLANT_NAME_MAPPER_CTOR 0x11797B4

#endif

class ZombieAlmanac
{
public:
    void* vftable;
    std::map<std::string, int> m_aliasToId;
};

class PlantNameMapper
{
public:
    void* vftable;
    std::map<std::string, int> m_aliasToId;
};

// 441/188 as for 9.6
#define FIRST_FREE_ZOMBIE_ID 441
#define FIRST_FREE_PLANT_ID 188

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
    REGISTER_ZOMBIE_TYPENAME("iceage_armor4");

    LOGI("Extra zombie typenames = %d", g_modZombieTypenames.size());

    for (int iter = 0; iter < g_modZombieTypenames.size(); iter++)
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
    REGISTER_PLANT_TYPENAME("tacticalcuke");
    REGISTER_PLANT_TYPENAME("icycurrant");

    LOGI("Extra typenames size = %d", g_modPlantTypenames.size());

    for (int iter = 0; iter < g_modPlantTypenames.size(); iter++)
    {
        LOGI("Registering plant %s", g_modPlantTypenames[iter].c_str());
        self->m_aliasToId[g_modPlantTypenames[iter]] = FIRST_FREE_PLANT_ID + iter;
    }

    return self;
}

#pragma endregion

#pragma region Mummy Memory Fix


#ifdef __arm__
#define OFFSET_CAMEL_ZOMBIE_FUNC 0x789DC8
#define CAMEL_MINIGAME_MODULE_FUNC 0x78CFA0

#elif __aarch64__
#define OFFSET_CAMEL_ZOMBIE_FUNC 0xB18DC4
#define CAMEL_MINIGAME_MODULE_FUNC 0xB1BE04

#endif

// The original function is broken for some reason. We don't need it
uint64_t oCamelZombieFunc = NULL;

typedef void(*camelMinigameModuleFunc)(int64_t, int64_t, int64_t);
camelMinigameModuleFunc cmmFunc = (camelMinigameModuleFunc)getActualOffset(CAMEL_MINIGAME_MODULE_FUNC);

void hkCamelZombieFunc(int64_t a1, int64_t a2, int64_t a3)
{
    // Redirect call to some function in CamelMinigameModule
    // This fixes the crash when camels are rising from the ground
    cmmFunc(a1, a2, a3);
}

#pragma endregion

#pragma region Vertical World Map Scrolling

#ifdef __arm__
#define OFFSET_WORLDMAP_DO_MOVEMENT 0x440E4C
#elif __aarch64__
#define OFFSET_WORLDMAP_DO_MOVEMENT 0x7AB6FC
#endif

bool g_allowVerticalMovement = true;

typedef int (*worldMapDoMovement)(WorldMap*, int64_t, int64_t);
worldMapDoMovement oWorldMapDoMovement = NULL;

int hkWorldMapDoMovement(WorldMap* map, int64_t x, int64_t y)
{
    LOGI("Doing map movement: x - %d, y - %d", x, y);
    return oWorldMapDoMovement(map, x, y);
}

inline int worldMapBoundaryMovement(WorldMap* self, float fX, float fY, bool allowVerticalMovement)
{
    if (fX <= self->m_boundaryX)
    {
        fX = self->m_boundaryX;
    }

    if (fX >= self->m_boundaryX + self->m_boundaryWidth)
    {
        fX = self->m_boundaryX + self->m_boundaryWidth;
    }

    if (fY <= self->m_boundaryY)
    {
        fY = self->m_boundaryY;
    }

    if (fY >= self->m_boundaryY + self->m_boundaryHeight)
    {
        fY = self->m_boundaryY + self->m_boundaryHeight;
    }

    return 1;
}

#pragma endregion

#pragma region Piano Zombie List

#ifdef __arm__
#define OFFSET_INIT_ZOMBIE_PIANO_LIST 0x885F80
#define INIT_PIANO_LIST_PTR 0x1D890F4
#elif __aarch64__
#define OFFSET_INIT_ZOMBIE_PIANO_LIST 0xC1D1FC
#define INIT_PIANO_LIST_PTR 0x2581BD0
#endif

typedef bool (*initZombiePianoList)(int64_t, int64_t);
initZombiePianoList oInitZombiePianoList = NULL;

bool g_pianoListInitialized = false;

std::vector<std::string>* g_pianoList;
bool hkInitZombiePianoList(int64_t a1, int64_t a2)
{
    // This function is called every frame when a piano zombie is on screen
    // So this global bool is needed to prevent wasting a massive amount of cpu time
    if (!g_pianoListInitialized)
    {
        bool orig = oInitZombiePianoList(a1, a2);

        uint64_t ptrAddr = getActualOffset(INIT_PIANO_LIST_PTR); // address of piano zombie's list in memory
        g_pianoList = reinterpret_cast<std::vector<std::string>*>(ptrAddr);

        // @todo: add this to piano zombie's props instead?
        g_pianoList->clear();
        g_pianoList->push_back("cowboy");
        g_pianoList->push_back("cowboy_armor1");
        g_pianoList->push_back("cowboy_armor2");
        g_pianoList->push_back("cowboy_armor4");
        g_pianoList->push_back("pirate_gargantuar");

        LOGI("Initialized global piano list");
        g_pianoListInitialized = true;
    }
    return oInitZombiePianoList(a1, a2);
}

#pragma endregion

#pragma region Board Zoom

#ifdef __arm__
#define OFFSET_REINIT_FOR_SURFACE_CHANGE 0x12873BC
#define OFFSET_BOARD_CTOR 0x70F63C

#elif __aarch64__
#define OFFSET_REINIT_FOR_SURFACE_CHANGE 0x16752B4
#define OFFSET_BOARD_CTOR 0xA99D24

#endif


#ifdef __arm__
#define LAWNAPP_ADDR 0x1D92A94
#define SEXYAPP_ADDR 0x1D9CA24

#elif __aarch64__
#define LAWNAPP_ADDR 0x2593760
#define SEXYAPP_ADDR 0x25A4618

#endif

int gWidth = 0;
int gHeight = 0;

inline uint64_t getLawnApp() {
    return *(uint64_t*)getActualOffset(LAWNAPP_ADDR);
}

uint64_t getSexyApp() {
    return *(uint64_t*)getActualOffset(SEXYAPP_ADDR);
}

enum AspectRatio
{
    Letterbox,
    Widescreen,
    Ultrawide,
};

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

typedef void(*ReinitForSurfaceChange)(int64_t, int64_t, int64_t, int64_t, int64_t);
ReinitForSurfaceChange oRFSC = nullptr;

void hkReinitForSurfaceChange(int64_t thisptr, int64_t a2, int64_t width, int64_t height, int64_t a5)
{
    gWidth = width;
    gHeight = height;
    return oRFSC(thisptr, a2, width, height, a5);
}

bool g_boardZoomOut = true;

typedef void* (*boardCtor)(Board*);
boardCtor oBoardCtor = NULL;

void* hkBoardCtor(Board* board)
{
    oBoardCtor(board);

    LOGI("Board constructor called");
    LOGI("[ Column Count ] x: %d", board->m_columnCount);
    LOGI("[ Row Count ] x: %d", board->m_rowCount);
    LOGI("[ Lawn Rect ] x: %d, y: %d, w: %d, h: %d", board->m_lawnRect.mX, board->m_lawnRect.mY, board->m_lawnRect.mWidth, board->m_lawnRect.mHeight);

    if (g_boardZoomOut)
    {
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
                // board->m_lawnRect.mHeight = 540; //380
                break;
            }
            case Ultrawide:
            {
                LOGI("Aspect Ratio: Ultrawide");
                board->m_lawnRect.mX = 450; //450
                board->m_lawnRect.mY = 160;
                board->m_lawnRect.mWidth = 576; //576
                board->m_lawnRect.mHeight = 525; //380
                break;
            }
        }
    }

    return board;
}

#pragma endregion

#pragma region Dumb Hardcoded Immunities (Healer/Magician)

#ifdef __arm__
#define OFFSET_MAGICIAN_IMMUNE 0x87A9E4
#define OFFSET_HEALER_IMMUNE 0x877AC4
#define OFFSET_MAGICIAN_CONDITION 0x8628B0
#define OFFSET_HEALER_CONDITION 0x8446A8
#define OFFSET_HEALER_INITIALIZE_FAMILY_IMMUNITIES 0x8628C4
#define Z_FAMILY_FUNC_ADDR 0x8BD2C0

#elif __aarch64__
#define OFFSET_MAGICIAN_IMMUNE 0xC12B7C
#define OFFSET_HEALER_IMMUNE 0xC0F91C
#define OFFSET_MAGICIAN_CONDITION 0xBF7A78
#define OFFSET_HEALER_CONDITION 0xBD8578
#define OFFSET_HEALER_INITIALIZE_FAMILY_IMMUNITIES 0xBF7BEC
#define Z_FAMILY_FUNC_ADDR 0xC5677C

#endif

// we don't need to save a pointer to the original functions
uint64_t dispose;

void hkMagicianHealerConditionFunc(int64_t a1, int64_t condition)
{
    // Hardcoded immunity checks would go here
    return;
}

bool hkMagicianHealerImmuneToShrink(int64_t a1)
{
    // true = immune to shrinking
    return false;
}

bool hkMagicianInitializeFamilyImmunities(int64_t a1, int64_t a2)
{
    typedef bool(*zFamilyFunc)(int64_t);
    zFamilyFunc func = (zFamilyFunc)getActualOffset(Z_FAMILY_FUNC_ADDR); // function 93 in Zombie's vftable 
    return func(a1);
}

#pragma endregion

__attribute__((constructor))
// This is automatically executed when the lib is loaded
// Run your initialization code here
void libPVZ2ExampleMod_main()
{
	LOGI("Initializing %s", LIB_TAG);

    // Function hooks
    PVZ2HookFunction(OFFSET_ZOMBIE_ALMANAC_CTOR, (void*)hkCreateZombieTypenameMap, (void**)&oZombieAlmanacCtor, "ZombieAlmanac::ZombieAlmanac");
    PVZ2HookFunction(OFFSET_PLANT_NAME_MAPPER_CTOR, (void*)hkCreatePlantNameMapper, (void**)&oPlantNameMapperCtor, "PlantNameMapper::PlantNameMapper");
    PVZ2HookFunction(OFFSET_CAMEL_ZOMBIE_FUNC, (void*)hkCamelZombieFunc, (void**)&oCamelZombieFunc, "CamelZombie::vftable_func_0xEC");
    //PVZ2HookFunction(OFFSET_WORLDMAP_DO_MOVEMENT, (void*)hkWorldMapDoMovement, (void**)&oWorldMapDoMovement, "WorldMap::doMovement");
    PVZ2HookFunction(OFFSET_INIT_ZOMBIE_PIANO_LIST, (void*)hkInitZombiePianoList, (void**)&oInitZombiePianoList, "ZombiePiano::getTypenameList");
    PVZ2HookFunction(OFFSET_REINIT_FOR_SURFACE_CHANGE, (void*)hkReinitForSurfaceChange, (void**)&oRFSC, "ReinitForSurfaceChanged");
    PVZ2HookFunction(OFFSET_BOARD_CTOR, (void*)hkBoardCtor, (void**)&oBoardCtor, "Board::Board");
    //PVZ2HookFunction(OFFSET_MAGICIAN_IMMUNE, (void*)hkMagicianHealerImmuneToShrink, (void**)&dispose, "ZombieCarnieMagician::IsImmuneToShrink");
    //PVZ2HookFunction(OFFSET_HEALER_IMMUNE, (void*)hkMagicianHealerImmuneToShrink, (void**)&dispose, "ZombieRomanHealer::IsImmuneToShrink");
    //PVZ2HookFunction(OFFSET_MAGICIAN_CONDITION, (void*)hkMagicianHealerConditionFunc, (void**)&dispose, "ZombieCarnieMagician::ConditionFunc");
    //PVZ2HookFunction(OFFSET_HEALER_CONDITION, (void*)hkMagicianHealerConditionFunc, (void**)&dispose, "ZombieRomanHealer::ConditionFunc");
    //PVZ2HookFunction(OFFSET_HEALER_INITIALIZE_FAMILY_IMMUNITIES, (void*)hkMagicianInitializeFamilyImmunities, (void**)&dispose, "ZombieRomanHealer::InitializeFamilyImmunities");

    LOGI("Finished initializing");
}
