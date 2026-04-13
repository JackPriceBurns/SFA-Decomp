#include "src/audio/synth_internal.h"

extern void fn_80278D74(void*);
extern u32 fn_80279C00(u32);
extern void fn_80282630(s32, s32, s32);
extern void fn_802836E4(s32*);
extern const f32 lbl_803E8430;
extern const f32 lbl_803E8440;
extern const f32 lbl_803E846C;

#define SYNTH_FADE_COUNT 0x20
#define SYNTH_FADE_SELECTOR_ACTION_2 0xFA
#define SYNTH_FADE_SELECTOR_ACTION_3 0xFB
#define SYNTH_FADE_SELECTOR_ACTION_2_OR_3 0xFC
#define SYNTH_FADE_SELECTOR_ACTION_0 0xFD
#define SYNTH_FADE_SELECTOR_ACTION_1 0xFE
#define SYNTH_FADE_SELECTOR_ACTION_0_OR_1 0xFF
#define SYNTH_FADE_ACTION_DISABLED 4
#define SYNTH_INVALID_LINK_ID 0xFFFFFFFF
#define SYNTH_FADE_SCALE lbl_803E8430
#define SYNTH_FADE_ONE lbl_803E8440
#define SYNTH_FADE_TIME_SCALE lbl_803E846C
#define SYNTH_FADE_TABLE gSynthFades

#define SYNTH_APPLY_FADE(fade, fadeIndex)     \
    do {                                      \
        (fade)->delayAction = action;         \
        (fade)->handle = handle;              \
                                              \
        if (fadeTime != 0) {                  \
            (fade)->start = (fade)->current;  \
            (fade)->target = target;          \
            (fade)->progress = SYNTH_FADE_ONE; \
            (fade)->progressStep = SYNTH_FADE_TIME_SCALE / (f32)fadeTime; \
        } else {                              \
            (fade)->target = target;          \
            (fade)->current = target;         \
                                              \
            if ((fade)->handle != SYNTH_INVALID_LINK_ID) { \
                synthDispatchDelayedAction(fade); \
            }                                 \
        }                                     \
                                              \
        gSynthFadeMask |= 1 << (fadeIndex);   \
    } while (0)

void fn_802721A0(s32 value0, s32 value1) {
    fn_80282630(7, value0, value1);
    fn_80282630(10, value0, value1);
    fn_80282630(0x5B, value0, value1);
    fn_80282630(0x80, value0, value1);
    fn_80282630(0x84, value0, value1);
}

s32 synthTriggerCallback(u32 callbackId) {
    u32 linkId;
    s32 handled;

    handled = 0;
    if (gSynthInitialized != 0) {
        for (linkId = fn_80279C00(callbackId); linkId != SYNTH_INVALID_LINK_ID;
             linkId = gSynthVoiceSlots[linkId & 0xFF].callbackNext) {
            SynthVoiceSlot* slot;

            slot = &gSynthVoiceSlots[linkId & 0xFF];
            if (linkId == slot->callbackLinkId) {
                fn_80278D74(slot);
                handled = 1;
            }
        }
    }

    return handled;
}

void synthSetFade(u8 value, u16 time, u8 selector, u8 action, u32 handle) {
    SynthFade* fadeTable;
    u32 fadeIndex;
    u32 fadeTime;
    u8 actionFilter;
    SynthFade* fade;
    f32 target;

    fadeTime = time;
    if (fadeTime != 0) {
        fn_802836E4((s32*)&fadeTime);
    }

    fadeTable = SYNTH_FADE_TABLE;
    target = (f32)value * SYNTH_FADE_SCALE;

    if (selector == SYNTH_FADE_SELECTOR_ACTION_0_OR_1) {
apply_actions_0_or_1:
        fade = fadeTable;
        for (fadeIndex = 0; fadeIndex < SYNTH_FADE_COUNT; fadeIndex++, fade++) {
            if (fade->type == 0 || fade->type == 1) {
                SYNTH_APPLY_FADE(fade, fadeIndex);
            }
        }
        return;
    }

    if (selector == SYNTH_FADE_SELECTOR_ACTION_2_OR_3) {
apply_actions_2_or_3:
        fade = fadeTable;
        for (fadeIndex = 0; fadeIndex < SYNTH_FADE_COUNT; fadeIndex++, fade++) {
            if (fade->type == 2 || fade->type == 3) {
                SYNTH_APPLY_FADE(fade, fadeIndex);
            }
        }
        return;
    }

    if (selector == SYNTH_FADE_SELECTOR_ACTION_0) {
        actionFilter = 0;
    } else if (selector < SYNTH_FADE_SELECTOR_ACTION_0) {
        if (selector == SYNTH_FADE_SELECTOR_ACTION_3) {
            actionFilter = 3;
        } else if (selector >= SYNTH_FADE_SELECTOR_ACTION_2_OR_3) {
            goto apply_actions_2_or_3;
        } else if (selector >= SYNTH_FADE_SELECTOR_ACTION_2) {
            actionFilter = 2;
        } else {
            fade = &fadeTable[selector];
            SYNTH_APPLY_FADE(fade, selector);
            return;
        }
    } else if (selector < SYNTH_FADE_SELECTOR_ACTION_0_OR_1) {
        actionFilter = 1;
    } else {
        fade = &fadeTable[selector];
        SYNTH_APPLY_FADE(fade, selector);
        return;
    }

    fade = fadeTable;
    for (fadeIndex = 0; fadeIndex < SYNTH_FADE_COUNT; fadeIndex++, fade++) {
        if (fade->type == actionFilter) {
            SYNTH_APPLY_FADE(fade, fadeIndex);
        }
    }
    return;
}

u32 synthIsFadeActive(u32 fadeIndex) {
    SynthFade* fade;
    u32 mask;

    fade = &SYNTH_FADE_TABLE[fadeIndex & 0xFF];
    mask = 1 << (fadeIndex & 0xFF);

    if (fade->type != SYNTH_FADE_ACTION_DISABLED && (gSynthFadeMask & mask) != 0 && fade->target < fade->start) {
        return 1;
    }

    return 0;
}

void synthSetFadeAction(u32 fadeIndex, u8 action) {
    if (gSynthInitialized == 0) {
        return;
    }

    gSynthFades[fadeIndex & 0xFF].type = action;
}

#undef SYNTH_APPLY_FADE
