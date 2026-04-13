/* TODO: restore stripped imported address metadata if needed. */

/**
 * GCN_mem_alloc.c
 * Description:
 */

#include "dolphin/os.h"

static const char gNoHeapMsg[] = "GCN_Mem_Alloc.c : InitDefaultHeap. No Heap Available\n";
static const char gRuntimeInitMsg[] = "Metrowerks CW runtime library initializing default heap\n";

inline static void InitDefaultHeap(void) {
	void* arenaLo;
	void* arenaHi;

	OSReport(gNoHeapMsg);
	OSReport(gRuntimeInitMsg);

	arenaLo = OSGetArenaLo();
	arenaHi = OSGetArenaHi();

	arenaLo = OSInitAlloc(arenaLo, arenaHi, 1);
	OSSetArenaLo(arenaLo);

	arenaLo = (void*)OSRoundUp32B(arenaLo);
	arenaHi = (void*)OSRoundDown32B(arenaHi);

	OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
	OSSetArenaLo(arenaLo = arenaHi);
}

void __sys_free(void* p) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }

    OSFreeToHeap(__OSCurrHeap, p);
}

void* __sys_alloc(u32 size) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }

    return OSAllocFromHeap(__OSCurrHeap, size);
}
