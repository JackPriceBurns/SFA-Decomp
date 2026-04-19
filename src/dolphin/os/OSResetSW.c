#include <dolphin.h>
#include <dolphin/os.h>

#include "dolphin/os/__os.h"

extern OSResetCallback bootThisDol_803DEAE8;
extern BOOL lbl_803DEAEC;
extern BOOL lbl_803DEAF0;
extern OSTime lbl_803DEAF8;
extern OSTime lbl_803DEB00;

void __OSResetSWInterruptHandler(s16 exception, OSContext* context) {
    OSResetCallback callback;

    lbl_803DEB00 = __OSGetSystemTime();
    while (__OSGetSystemTime() - lbl_803DEB00 < OSMicrosecondsToTicks(100) &&
           !(__PIRegs[0] & 0x00010000)) {
        ;
    }
    if (!(__PIRegs[0] & 0x00010000)) {
        lbl_803DEAF0 = lbl_803DEAEC = TRUE;
        __OSMaskInterrupts(OS_INTERRUPTMASK_PI_RSW);
        if (bootThisDol_803DEAE8) {
            callback = bootThisDol_803DEAE8;
            bootThisDol_803DEAE8 = NULL;
            callback();
        }
    }
    __PIRegs[0] = 2;
}

BOOL OSGetResetButtonState(void) {
    BOOL enabled = OSDisableInterrupts();
    int state;
    u32 reg;
    OSTime now;

    now = __OSGetSystemTime();

    reg = __PIRegs[0];
    if (!(reg & 0x00010000)) {
        if (!lbl_803DEAEC) {
            lbl_803DEAEC = TRUE;
            state = lbl_803DEAF8 ? TRUE : FALSE;
            lbl_803DEB00 = now;
        } else {
            state = lbl_803DEAF8 || (OSMicrosecondsToTicks(100) < now - lbl_803DEB00)
                        ? TRUE
                        : FALSE;
        }
    } else if (lbl_803DEAEC) {
        lbl_803DEAEC = FALSE;
        state = lbl_803DEAF0;
        if (state) {
            lbl_803DEAF8 = now;
        } else {
            lbl_803DEAF8 = 0;
        }
    } else if (lbl_803DEAF8 && (now - lbl_803DEAF8 < OSMillisecondsToTicks(40))) {
        state = TRUE;
    } else {
        state = FALSE;
        lbl_803DEAF8 = 0;
    }

    lbl_803DEAF0 = state;

    if (__gUnknown800030E3 & 0x3F) {
        OSTime fire = (__gUnknown800030E3 & 0x3F) * 60;
        fire = __OSStartTime + OSSecondsToTicks(fire);
        if (fire < now) {
            now -= fire;
            now = OSTicksToSeconds(now) / 2;
            if ((now & 1) == 0) {
                state = TRUE;
            } else {
                state = FALSE;
            }
        }
    }

    OSRestoreInterrupts(enabled);
    return state;
}
