#include "src/audio/synth_internal.h"

void synthInitVoices(void) {
    SynthCallbackLink* callback;
    SynthCallbackLink* prevCallback;
    u16* note;
    SynthVoice* prevVoice;
    SynthVoice* voice;
    s32 callbackIndex;
    s32 voiceIndex;

    gSynthAllocatedVoices = 0;
    gSynthQueuedVoices = 0;
    gSynthFreeVoices = &gSynthVoices[0];

    voice = &gSynthVoices[0];
    note = &gSynthVoiceNotes[0][0];
    prevVoice = 0;
    for (voiceIndex = 0; voiceIndex < SYNTH_MAX_VOICES; voiceIndex += 2) {
        voice->prev = prevVoice;
        if (prevVoice != 0) {
            prevVoice->next = voice;
        }
        voice->slotIndex = (u8)voiceIndex;
        voice->state = 0;

        note[0] = 0xFFFF;
        note[1] = 0xFFFF;
        note[2] = 0xFFFF;
        note[3] = 0xFFFF;
        note[4] = 0xFFFF;
        note[5] = 0xFFFF;
        note[6] = 0xFFFF;
        note[7] = 0xFFFF;
        note[8] = 0xFFFF;
        note[9] = 0xFFFF;
        note[10] = 0xFFFF;
        note[11] = 0xFFFF;
        note[12] = 0xFFFF;
        note[13] = 0xFFFF;
        note[14] = 0xFFFF;
        note[15] = 0xFFFF;

        prevVoice = voice;
        voice++;
        note += SYNTH_VOICE_NOTE_COUNT;

        voice->prev = prevVoice;
        prevVoice->next = voice;
        voice->slotIndex = (u8)(voiceIndex + 1);
        voice->state = 0;

        note[0] = 0xFFFF;
        note[1] = 0xFFFF;
        note[2] = 0xFFFF;
        note[3] = 0xFFFF;
        note[4] = 0xFFFF;
        note[5] = 0xFFFF;
        note[6] = 0xFFFF;
        note[7] = 0xFFFF;
        note[8] = 0xFFFF;
        note[9] = 0xFFFF;
        note[10] = 0xFFFF;
        note[11] = 0xFFFF;
        note[12] = 0xFFFF;
        note[13] = 0xFFFF;
        note[14] = 0xFFFF;
        note[15] = 0xFFFF;

        prevVoice = voice;
        voice++;
        note += SYNTH_VOICE_NOTE_COUNT;
    }
    prevVoice->next = 0;

    gSynthFreeCallbacks = &gSynthCallbacks[0];
    prevCallback = 0;
    for (callbackIndex = 0; callbackIndex < SYNTH_CALLBACK_COUNT; callbackIndex++) {
        callback = &gSynthCallbacks[callbackIndex];
        callback->prev = prevCallback;
        if (prevCallback != 0) {
            prevCallback->next = callback;
        }
        prevCallback = callback;
    }
    prevCallback->next = 0;

    gSynthNextHandle = 0;
}
