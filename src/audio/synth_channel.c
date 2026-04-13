#include "src/audio/synth_internal.h"

extern void fn_8026FCA0(s32 value, u8 studioIndex, u32 channelIndex);
extern u32 lbl_803DEEA0;

typedef struct SynthPitchPoint {
    u32 threshold;
    u32 value;
} SynthPitchPoint;

#define SYNTH_CHANNEL_STATE(voice, channel) ((u8*)&(voice)->unkEE0[0x608 + (((channel) & 0xFF) * 0x38)])
#define SYNTH_CHANNEL_EVENT_ACTIVE(state) (*(s32*)((state) + 0x00))
#define SYNTH_CHANNEL_EVENT_CURSOR(state) (*(SynthPitchPoint**)((state) + 0x04))
#define SYNTH_CHANNEL_CURRENT_VALUE(state) (*(u32*)((state) + 0x08))
#define SYNTH_CHANNEL_THRESHOLD_INDEX(state) (*(u8*)((state) + 0x30))
#define SYNTH_CHANNEL_THRESHOLD_VALUE(state, index) (*(u32*)((state) + 0x24 + ((index) * 8)))
#define SYNTH_VOICE_PROGRAM_DATA(voice) (*(u8**)((voice)->unk10 + 0x108))
#define SYNTH_PROGRAM_FLAGS(program) (*(u32*)((program) + 0x10))

void fn_8026D6DC(u32 channelIndex) {
    u8* channelState;
    SynthPitchPoint* point;

    channelState = SYNTH_CHANNEL_STATE(gSynthCurrentVoice, channelIndex);
    if (SYNTH_CHANNEL_EVENT_ACTIVE(channelState) == 0) {
        return;
    }

    while (1) {
        point = SYNTH_CHANNEL_EVENT_CURSOR(channelState);
        if (point->threshold == 0xFFFFFFFF ||
            point->threshold > SYNTH_CHANNEL_THRESHOLD_VALUE(
                                   channelState, SYNTH_CHANNEL_THRESHOLD_INDEX(channelState))) {
            break;
        }

        if ((SYNTH_PROGRAM_FLAGS(SYNTH_VOICE_PROGRAM_DATA(gSynthCurrentVoice)) & 0x40000000) != 0) {
            SYNTH_CHANNEL_CURRENT_VALUE(channelState) = point->value;
            fn_8026FCA0((s32)(point->value >> 10), (u8)lbl_803DEEA0, channelIndex);
        } else {
            fn_8026FCA0((s32)point->value, (u8)lbl_803DEEA0, channelIndex);
            point = SYNTH_CHANNEL_EVENT_CURSOR(channelState);
            SYNTH_CHANNEL_CURRENT_VALUE(channelState) = point->value << 10;
        }

        point = SYNTH_CHANNEL_EVENT_CURSOR(channelState);
        SYNTH_CHANNEL_EVENT_CURSOR(channelState) = point + 1;
    }
}
