#include "src/audio/synth_internal.h"

void synthInitVoices(void) {
    s32 callbackIndex;
    s32 noteIndex;
    SynthCallbackLink* callback;
    SynthCallbackLink* prevCallback;
    SynthVoice* voice;
    s32 voiceIndex;

    gSynthAllocatedVoices = 0;
    gSynthQueuedVoices = 0;
    gSynthFreeVoices = &gSynthVoices[0];

    for (voiceIndex = 0; voiceIndex < SYNTH_MAX_VOICES; voiceIndex++) {
        voice = &gSynthVoices[voiceIndex];
        voice->next = voiceIndex + 1 < SYNTH_MAX_VOICES ? &gSynthVoices[voiceIndex + 1] : 0;
        voice->prev = voiceIndex > 0 ? &gSynthVoices[voiceIndex - 1] : 0;
        voice->slotIndex = (u8)voiceIndex;
        voice->state = 0;

        for (noteIndex = 0; noteIndex < SYNTH_VOICE_NOTE_COUNT; noteIndex++) {
            gSynthVoiceNotes[voiceIndex][noteIndex] = 0xFFFF;
        }
    }

    gSynthFreeCallbacks = &gSynthCallbacks[0];
    prevCallback = 0;
    for (callbackIndex = 0; callbackIndex < SYNTH_CALLBACK_COUNT; callbackIndex++) {
        callback = &gSynthCallbacks[callbackIndex];
        callback->next = callbackIndex + 1 < SYNTH_CALLBACK_COUNT ? &gSynthCallbacks[callbackIndex + 1] : 0;
        callback->prev = prevCallback;
        prevCallback = callback;
    }

    gSynthNextHandle = 0;
}
