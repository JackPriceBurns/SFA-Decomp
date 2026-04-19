#include "PowerPC_EABI_Support/MetroTRK/trk.h"

extern u8 lbl_803DB710[8];

void SetUseSerialIO(u8 sio) {
    lbl_803DB710[0] = sio;
}

u8 GetUseSerialIO(void) {
    return lbl_803DB710[0];
}
