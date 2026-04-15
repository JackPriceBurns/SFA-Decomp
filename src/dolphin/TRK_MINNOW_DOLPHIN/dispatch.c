#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/dispatch.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msghndlr.h"

u32 gTRKDispatchTableSize;

struct DispatchEntry {
    DSError (*fn)(TRKBuffer*);
};

struct DispatchEntry gTRKDispatchTable[33] = {
    { &TRKDoUnsupported },   { &TRKDoConnect },        { &TRKDoDisconnect },
    { &TRKDoReset },         { &TRKDoVersions },       { &TRKDoSupportMask },
    { &TRKDoUnsupported },   { &TRKDoUnsupported },    { &TRKDoUnsupported },
    { &TRKDoUnsupported },   { &TRKDoUnsupported },    { &TRKDoUnsupported },
    { &TRKDoUnsupported },   { &TRKDoUnsupported },    { &TRKDoUnsupported },
    { &TRKDoUnsupported },   { &TRKDoReadMemory },     { &TRKDoWriteMemory },
    { &TRKDoReadRegisters }, { &TRKDoWriteRegisters }, { &TRKDoUnsupported },
    { &TRKDoUnsupported },   { &TRKDoUnsupported },    { &TRKDoSetOption },
    { &TRKDoContinue },      { &TRKDoStep },           { &TRKDoStop },
    { &TRKDoUnsupported },   { &TRKDoUnsupported },    { &TRKDoUnsupported },
    { &TRKDoUnsupported },   { &TRKDoUnsupported },
};

DSError TRKInitializeDispatcher(void)
{
    gTRKDispatchTableSize = 32;
    return DS_NoError;
}

BOOL TRKDispatchMessage(TRKBuffer* buffer)
{
    DSError error;
    u8 command;

    error = DS_DispatchError;
    TRKSetBufferPosition(buffer, 0);
    TRKReadBuffer1_ui8(buffer, &command);
    command &= 0xFF;
    if (command < gTRKDispatchTableSize) {
        error = gTRKDispatchTable[command].fn(buffer);
    }
    return error;
}
