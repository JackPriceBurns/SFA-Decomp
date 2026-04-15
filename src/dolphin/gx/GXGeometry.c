#include <dolphin/gx.h>

#include "dolphin/gx/__gx.h"

void __GXSetMatrixIndex(GXAttr matIdxAttr) {
    if (matIdxAttr < GX_VA_TEX4MTXIDX) {
        GX_WRITE_SOME_REG4(8, 0x30, __GXData->matIdxA, -12);
        GX_WRITE_XF_REG(24, __GXData->matIdxA);
    } else {
        GX_WRITE_SOME_REG4(8, 0x40, __GXData->matIdxB, -12);
        GX_WRITE_XF_REG(25, __GXData->matIdxB);
    }

    __GXData->bpSentNot = GX_TRUE;
}
