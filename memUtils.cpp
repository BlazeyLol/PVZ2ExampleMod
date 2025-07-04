#include "memUtils.hpp"
#include "Logging.hpp"

#ifdef __arm__
#include "Hooks/Substrate/SubstrateHook.h"
#elif __aarch64__
#include "Hooks/And64Inline/And64InlineHook.hpp"
#endif

uint64_t g_libAddress = NULL;

uint64_t getLibraryAddress(const char* libName) {
    FILE* fp = fopen("/proc/self/maps", "rt");
    if (fp == NULL) {
        perror("fopen");
        return 0;
    }

    uint64_t addr = 0;
    char line[1024];

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, libName) != NULL) {
            addr = strtoul(line, NULL, 16);
            LOGI("%s at 0x%x", libName, addr);
            break;
        }
    }

    fclose(fp);
    return addr;
}

uint64_t getActualOffset(uint64_t offset)
{
    if (g_libAddress == 0)
    {
        g_libAddress = getLibraryAddress("libPVZ2.so");
    }
    return g_libAddress + offset;
}

uint64_t getOriginalOffset(uint64_t actualOffset) {
    if (g_libAddress == 0)
    {
        g_libAddress = getLibraryAddress("libPVZ2.so");
    }
    return actualOffset - g_libAddress;
}

#if __arm__
void PVZ2HookFunction(uint64_t offset, void* replace, void** result, const char* funcName)
{
    MSHookFunction((void*)getActualOffset(offset), replace, result);
    LOGI("Hooked %s", funcName);
}

#elif __aarch64__

void PVZ2HookFunction(uint64_t offset, void* replace, void** result, const char* funcName)
{
    A64HookFunction((void*)getActualOffset(offset), replace, result);
    LOGI("Hooked %s", funcName);
}
#endif

void* copyVFTable(int vftableAddr, int numVFuncs)
{
    int size = numVFuncs * sizeof(int);
    void* vftableCopy = malloc(size);
    memcpy(vftableCopy, (const void*)vftableAddr, size);
    return vftableCopy;
}

void patchVFTable(void* vftable, void* funcAddr, int index)
{
    ((reinterpret_cast<void**>(vftable))[index]) = funcAddr;
}

void copyVFTable(void* dest, int vftableAddr, int numVFuncsToCopy) {
    int size = numVFuncsToCopy * sizeof(int);
    memcpy(dest, (const void*)vftableAddr, size);
}

void* createChildVFTable(int vFuncsCount, int parentVftable, int nuMVFuncsToCopy) {
    int size = vFuncsCount * sizeof(int);
    void* childVftable = malloc(size);
    copyVFTable(childVftable, parentVftable, nuMVFuncsToCopy);

    return childVftable;
}

void setVFTable(void* obj, uintptr_t newVftablePtr)
{
    *reinterpret_cast<int*>(uintptr_t(obj)) = newVftablePtr;
}

void* GetVirtualFunc(void* obj, int index) {
    void** vtable = *reinterpret_cast<void***>(obj);
    return vtable[index];
}
