#include "ZombiePiano.hpp"

typedef bool (*initZombiePianoList)(INT, INT);
initZombiePianoList oInitZombiePianoList = NULL;

bool g_pianoListInitialized = false;

std::vector<std::string>* g_pianoList;
bool hkInitZombiePianoList(INT a1, INT a2)
{
    // This function is called every frame when a piano zombie is on screen
    // So this global bool is needed to prevent wasting a massive amount of cpu time
    if (!g_pianoListInitialized)
    {
        bool orig = oInitZombiePianoList(a1, a2);

        UINT ptrAddr = getActualOffset(INIT_PIANO_LIST_PTR); // address of piano zombie's list in memory
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

void ZombiePiano::ZombiePianoHooks()
{
    LOGI("Init Feature: Custom Piano List");
	PVZ2HookFunction(OFFSET_INIT_ZOMBIE_PIANO_LIST, (void*)hkInitZombiePianoList, (void**)&oInitZombiePianoList, "ZombiePiano::getTypenameList");
}