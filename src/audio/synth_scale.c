#include "src/audio/synth_internal.h"

#define SYNTH_DEFAULT_STUDIO_INDEX 8
#define SYNTH_SLOT_STUDIO_INDEX(slot) (((u8*)(slot))[0x122])
#define SYNTH_SLOT_CHANNEL_INDEX(slot) (((u8*)(slot))[0x123])

void synthSetStudioChannelScale(s32 value, u8 studioIndex, u32 channelIndex) {
    u8* scale;

    if (studioIndex == 0xFF) {
        studioIndex = SYNTH_DEFAULT_STUDIO_INDEX;
    }

    scale = (u8*)&gSynthDelayStorage + ((studioIndex & 0xFF) << 6);
    scale += (channelIndex & 0xFF) << 2;
    *(u32*)scale = ((((u32)value) << 3) * 0x600) / 0xF0;
}

u32 synthGetVoiceSlotChannelScale(SynthVoiceSlot* slot) {
    u8* scale;
    u8 studioIndex;

    studioIndex = SYNTH_SLOT_STUDIO_INDEX(slot);
    if (studioIndex == 0xFF) {
        studioIndex = SYNTH_DEFAULT_STUDIO_INDEX;
    }

    scale = (u8*)&gSynthDelayStorage + (studioIndex << 6);
    scale += SYNTH_SLOT_CHANNEL_INDEX(slot) << 2;
    return *(u32*)scale;
}
