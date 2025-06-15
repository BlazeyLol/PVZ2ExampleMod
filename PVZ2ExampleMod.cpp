#include "PVZ2ExampleMod.hpp"
#include "PvZ2/NameMapper.hpp"
#include "PvZ2/ZombieCamel.hpp"
#include "PvZ2/WorldMap.hpp"
#include "PvZ2/ZombiePiano.hpp"
#include "PvZ2/Board.hpp"
#include "PvZ2/ImmunityMgr.hpp"

__attribute__((constructor))
// This is automatically executed when the lib is loaded
// Run your initialization code here
void libPVZ2ExampleMod_main()
{
	LOGI("Initializing %s", LIB_TAG);

    // Function hooks
    NameMapperBase::NameMapperHooks();
    ZombieCamel::ZombieCamelHooks();
    //WorldMap::WorldMapHooks();
    ZombiePiano::ZombiePianoHooks();
    //Board::BoardHooks();
    //ImmunityMgr::ImmunityRemovalHook();

    LOGI("Finished initializing");
}