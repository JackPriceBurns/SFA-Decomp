#include <dolphin/gx.h>
#include <dolphin/os.h>

#include "dolphin/gx/__gx.h"

void GXSetCopyFilter(GXBool aa, const u8 sample_pattern[12][2], GXBool vf, const u8 vfilter[7]) {
    u32 msLoc[4];
    u32 coeff0;
    u32 coeff1;

    CHECK_GXBEGIN(1641, "GXSetCopyFilter");

    if (aa != 0) {
        msLoc[0] = 0;
        SET_REG_FIELD(0, msLoc[0], 4, 0, sample_pattern[0][0]);
        SET_REG_FIELD(0, msLoc[0], 4, 4, sample_pattern[0][1]);
        SET_REG_FIELD(0, msLoc[0], 4, 8, sample_pattern[1][0]);
        SET_REG_FIELD(0, msLoc[0], 4, 12, sample_pattern[1][1]);
        SET_REG_FIELD(0, msLoc[0], 4, 16, sample_pattern[2][0]);
        SET_REG_FIELD(0, msLoc[0], 4, 20, sample_pattern[2][1]);
        SET_REG_FIELD(0, msLoc[0], 8, 24, 1);

        msLoc[1] = 0;
        SET_REG_FIELD(0, msLoc[1], 4, 0, sample_pattern[3][0]);
        SET_REG_FIELD(0, msLoc[1], 4, 4, sample_pattern[3][1]);
        SET_REG_FIELD(0, msLoc[1], 4, 8, sample_pattern[4][0]);
        SET_REG_FIELD(0, msLoc[1], 4, 12, sample_pattern[4][1]);
        SET_REG_FIELD(0, msLoc[1], 4, 16, sample_pattern[5][0]);
        SET_REG_FIELD(0, msLoc[1], 4, 20, sample_pattern[5][1]);
        SET_REG_FIELD(0, msLoc[1], 8, 24, 2);

        msLoc[2] = 0;
        SET_REG_FIELD(0, msLoc[2], 4, 0, sample_pattern[6][0]);
        SET_REG_FIELD(0, msLoc[2], 4, 4, sample_pattern[6][1]);
        SET_REG_FIELD(0, msLoc[2], 4, 8, sample_pattern[7][0]);
        SET_REG_FIELD(0, msLoc[2], 4, 12, sample_pattern[7][1]);
        SET_REG_FIELD(0, msLoc[2], 4, 16, sample_pattern[8][0]);
        SET_REG_FIELD(0, msLoc[2], 4, 20, sample_pattern[8][1]);
        SET_REG_FIELD(0, msLoc[2], 8, 24, 3);

        msLoc[3] = 0;
        SET_REG_FIELD(0, msLoc[3], 4, 0, sample_pattern[9][0]);
        SET_REG_FIELD(0, msLoc[3], 4, 4, sample_pattern[9][1]);
        SET_REG_FIELD(0, msLoc[3], 4, 8, sample_pattern[10][0]);
        SET_REG_FIELD(0, msLoc[3], 4, 12, sample_pattern[10][1]);
        SET_REG_FIELD(0, msLoc[3], 4, 16, sample_pattern[11][0]);
        SET_REG_FIELD(0, msLoc[3], 4, 20, sample_pattern[11][1]);
        SET_REG_FIELD(0, msLoc[3], 8, 24, 4);
    } else {
        msLoc[0] = 0x01666666;
        msLoc[1] = 0x02666666;
        msLoc[2] = 0x03666666;
        msLoc[3] = 0x04666666;
    }

    GX_WRITE_RAS_REG(msLoc[0]);
    GX_WRITE_RAS_REG(msLoc[1]);
    GX_WRITE_RAS_REG(msLoc[2]);
    GX_WRITE_RAS_REG(msLoc[3]);

    coeff0 = 0;
    SET_REG_FIELD(0, coeff0, 8, 24, 0x53);
    coeff1 = 0;
    SET_REG_FIELD(0, coeff1, 8, 24, 0x54);
    if (vf != 0) {
        SET_REG_FIELD(0, coeff0, 6, 0, vfilter[0]);
        SET_REG_FIELD(0, coeff0, 6, 6, vfilter[1]);
        SET_REG_FIELD(0, coeff0, 6, 12, vfilter[2]);
        SET_REG_FIELD(0, coeff0, 6, 18, vfilter[3]);
        SET_REG_FIELD(0, coeff1, 6, 0, vfilter[4]);
        SET_REG_FIELD(0, coeff1, 6, 6, vfilter[5]);
        SET_REG_FIELD(0, coeff1, 6, 12, vfilter[6]);
    } else {
        SET_REG_FIELD(0, coeff0, 6, 0, 0);
        SET_REG_FIELD(0, coeff0, 6, 6, 0);
        SET_REG_FIELD(0, coeff0, 6, 12, 21);
        SET_REG_FIELD(0, coeff0, 6, 18, 22);
        SET_REG_FIELD(0, coeff1, 6, 0, 21);
        SET_REG_FIELD(0, coeff1, 6, 6, 0);
        SET_REG_FIELD(0, coeff1, 6, 12, 0);
    }

    GX_WRITE_RAS_REG(coeff0);
    GX_WRITE_RAS_REG(coeff1);
    __GXData->bpSentNot = 0;
}

void GXSetDispCopyGamma(GXGamma gamma) {
    CHECK_GXBEGIN(1741, "GXSetDispCopyGamma");
    __GXData->cpDisp = (__GXData->cpDisp & 0xFFFFFE7F) | ((u32)gamma << 7);
}

void GXCopyDisp(void* dest, GXBool clear) {
    u32 reg;
    u32 tempPeCtrl;
    u32 phyAddr;
    u8 changePeCtrl;

    CHECK_GXBEGIN(1833, "GXCopyDisp");

    if (clear) {
        reg = __GXData->zmode;
        SET_REG_FIELD(0, reg, 1, 0, 1);
        SET_REG_FIELD(0, reg, 3, 1, 7);
        GX_WRITE_RAS_REG(reg);

        reg = __GXData->cmode0;
        SET_REG_FIELD(0, reg, 1, 0, 0);
        SET_REG_FIELD(0, reg, 1, 1, 0);
        GX_WRITE_RAS_REG(reg);
    }

    changePeCtrl = FALSE;
    if ((clear || (u32)GET_REG_FIELD(__GXData->peCtrl, 3, 0) == 3)
        && (u32)GET_REG_FIELD(__GXData->peCtrl, 1, 6) == 1) {
        changePeCtrl = TRUE;
        tempPeCtrl = __GXData->peCtrl;
        SET_REG_FIELD(0, tempPeCtrl, 1, 6, 0);
        GX_WRITE_RAS_REG(tempPeCtrl);
    }

    GX_WRITE_RAS_REG(__GXData->cpDispSrc);
    GX_WRITE_RAS_REG(__GXData->cpDispSize);
    GX_WRITE_RAS_REG(__GXData->cpDispStride);

    phyAddr = (u32)dest & 0x3FFFFFFF;
    reg = 0;
    SET_REG_FIELD(0, reg, 21, 0, phyAddr >> 5);
    SET_REG_FIELD(0, reg, 8, 24, 0x4B);
    GX_WRITE_RAS_REG(reg);

    SET_REG_FIELD(0, __GXData->cpDisp, 1, 11, clear);
    SET_REG_FIELD(0, __GXData->cpDisp, 1, 14, 1);
    SET_REG_FIELD(0, __GXData->cpDisp, 8, 24, 0x52);
    GX_WRITE_RAS_REG(__GXData->cpDisp);

    if (clear) {
        GX_WRITE_RAS_REG(__GXData->zmode);
        GX_WRITE_RAS_REG(__GXData->cmode0);
    }

    if (changePeCtrl) {
        GX_WRITE_RAS_REG(__GXData->peCtrl);
    }

    __GXData->bpSentNot = 0;
}

void GXCopyTex(void* dest, GXBool clear) {
    u32 reg;
    u32 tempPeCtrl;
    u32 phyAddr;
    u8 changePeCtrl;

    CHECK_GXBEGIN(1916, "GXCopyTex");

    if (clear != 0) {
        reg = __GXData->zmode;
        SET_REG_FIELD(0, reg, 1, 0, 1);
        SET_REG_FIELD(0, reg, 3, 1, 7);
        GX_WRITE_RAS_REG(reg);

        reg = __GXData->cmode0;
        SET_REG_FIELD(0, reg, 1, 0, 0);
        SET_REG_FIELD(0, reg, 1, 1, 0);
        GX_WRITE_RAS_REG(reg);
    }

    changePeCtrl = 0;
    tempPeCtrl = __GXData->peCtrl;

    if (((u8)__GXData->cpTexZ != 0) && ((u32)(tempPeCtrl & 7) != 3)) {
        changePeCtrl = 1;
        tempPeCtrl = (tempPeCtrl & 0xFFFFFFF8) | 3;
    }

    if (((clear != 0) || ((u32)(tempPeCtrl & 7) == 3)) && ((u32)((tempPeCtrl >> 6U) & 1) == 1)) {
        changePeCtrl = 1;
        tempPeCtrl &= 0xFFFFFFBF;
    }

    if (changePeCtrl) {
        GX_WRITE_RAS_REG(tempPeCtrl);
    }

    GX_WRITE_RAS_REG(__GXData->cpTexSrc);
    GX_WRITE_RAS_REG(__GXData->cpTexSize);
    GX_WRITE_RAS_REG(__GXData->cpTexStride);

    phyAddr = (u32)dest & 0x3FFFFFFF;
    reg = 0;
    SET_REG_FIELD(0, reg, 21, 0, phyAddr >> 5);
    SET_REG_FIELD(0, reg, 8, 24, 0x4B);
    GX_WRITE_RAS_REG(reg);

    SET_REG_FIELD(0, __GXData->cpTex, 1, 11, clear);
    SET_REG_FIELD(0, __GXData->cpTex, 1, 14, 0);
    SET_REG_FIELD(0, __GXData->cpTex, 8, 24, 0x52);
    GX_WRITE_RAS_REG(__GXData->cpTex);

    if (clear != 0) {
        GX_WRITE_RAS_REG(__GXData->zmode);
        GX_WRITE_RAS_REG(__GXData->cmode0);
    }

    if (changePeCtrl) {
        GX_WRITE_RAS_REG(__GXData->peCtrl);
    }

    __GXData->bpSentNot = 0;
}

void GXClearBoundingBox(void) {
    u32 reg;

    CHECK_GXBEGIN(2003, "GXClearBoundingBox");
    reg = 0x550003FF;
    GX_WRITE_RAS_REG(reg);
    reg = 0x560003FF;
    GX_WRITE_RAS_REG(reg);
    __GXData->bpSentNot = 0;
}
