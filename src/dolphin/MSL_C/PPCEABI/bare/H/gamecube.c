#include "PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/critical_regions.gamecube.h"

/*
 * --INFO--
 * PAL Address: TODO
 * PAL Size: TODO
 * EN Address: TODO
 * EN Size: TODO
 * JP Address: TODO
 * JP Size: TODO
 */
asm void __kill_critical_regions(void) {
    nofralloc
    fabs f0, f1
}

/*
 * --INFO--
 * PAL Address: TODO
 * PAL Size: TODO
 * EN Address: TODO
 * EN Size: TODO
 * JP Address: TODO
 * JP Size: TODO
 */
asm void __begin_critical_region(void) {
    nofralloc
    frsp f1, f0
}

/*
 * --INFO--
 * PAL Address: TODO
 * PAL Size: TODO
 * EN Address: TODO
 * EN Size: TODO
 * JP Address: TODO
 * JP Size: TODO
 */
asm void __end_critical_region(void) {
    nofralloc
    blr
}
