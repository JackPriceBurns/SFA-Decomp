#include "src/audio/synth_internal.h"

extern void synthRecycleVoiceCallbacks(SynthVoice*);

#define SYNTH_VOICE_STATE_FREE 0
#define SYNTH_VOICE_STATE_ALLOCATED 1
#define SYNTH_VOICE_STATE_QUEUED 2
#define SYNTH_PENDING_FLAG_QUEUE 8
#define SYNTH_PENDING_FLAG_MIX_DATA 0x10
#define SYNTH_PENDING_FLAG_VALUE16 0x20
#define SYNTH_PENDING_FLAG_UPDATE_MODE3 0x80

#define SYNTH_CHANNEL_VALUE16(voice, channel) (*(u16*)&((voice)->channelData[((channel) * 0x38) + 2]))
#define SYNTH_HANDLE_SLOT_INVALID 0xFFFFFFFF

void synthQueueVoice(SynthVoice* voice) {
    if (voice->prev != 0) {
        voice->prev->next = voice->next;
    } else {
        gSynthAllocatedVoices = voice->next;
    }

    if (voice->next != 0) {
        voice->next->prev = voice->prev;
    }

    voice->next = gSynthQueuedVoices;
    if (gSynthQueuedVoices != 0) {
        gSynthQueuedVoices->prev = voice;
    }

    voice->prev = 0;
    gSynthQueuedVoices = voice;
    voice->state = SYNTH_VOICE_STATE_QUEUED;
}

void synthQueueHandle(u32 handle) {
    u32 resolvedHandle;
    SynthVoice* allocatedVoice;
    u32 listIndex;
    SynthCallbackLink* callback;
    u32 slot;
    SynthVoice* queuedVoice;
    SynthVoice* voice;

    resolvedHandle = handle & 0x7FFFFFFF;
    for (allocatedVoice = gSynthAllocatedVoices; allocatedVoice != 0; allocatedVoice = allocatedVoice->next) {
        if (allocatedVoice->handle == resolvedHandle) {
            slot = allocatedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    for (queuedVoice = gSynthQueuedVoices; queuedVoice != 0; queuedVoice = queuedVoice->next) {
        if (queuedVoice->handle == resolvedHandle) {
            slot = queuedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    slot = SYNTH_HANDLE_SLOT_INVALID;

found:
    if (slot == SYNTH_HANDLE_SLOT_INVALID) {
        return;
    }

    if ((slot & 0x80000000) == 0) {
        voice = &gSynthVoices[slot];
        if (voice->state != SYNTH_VOICE_STATE_ALLOCATED) {
            return;
        }

        if (voice->prev != 0) {
            voice->prev->next = voice->next;
        } else {
            gSynthAllocatedVoices = voice->next;
        }

        if (voice->next != 0) {
            voice->next->prev = voice->prev;
        }

        voice->next = gSynthQueuedVoices;
        if (gSynthQueuedVoices != 0) {
            gSynthQueuedVoices->prev = voice;
        }

        voice->prev = 0;
        gSynthQueuedVoices = voice;
        voice->state = SYNTH_VOICE_STATE_QUEUED;

        for (listIndex = 0; listIndex < 2; listIndex++) {
            for (callback = voice->callbackLists[listIndex]; callback != 0; callback = callback->next) {
                synthCancelCallbackVoices(callback->callbackId);
            }
        }

        for (callback = voice->callbackLists[2]; callback != 0; callback = callback->next) {
            synthCancelCallbackVoices(callback->callbackId);
        }

        synthRecycleVoiceCallbacks(voice);
        return;
    }

    voice = &gSynthVoices[slot & 0x7FFFFFFF];
    if (voice->state != SYNTH_VOICE_STATE_FREE) {
        voice->pendingUpdate.flags |= SYNTH_PENDING_FLAG_QUEUE;
    }
}

void synthFreeHandle(u32 handle) {
    u32 resolvedHandle;
    u8 state;
    u32 listIndex;
    SynthCallbackLink* callback;
    SynthVoice* allocatedVoice;
    u32 slot;
    SynthVoice* queuedVoice;
    SynthVoice* voice;

    resolvedHandle = handle & 0x7FFFFFFF;
    for (allocatedVoice = gSynthAllocatedVoices; allocatedVoice != 0; allocatedVoice = allocatedVoice->next) {
        if (allocatedVoice->handle == resolvedHandle) {
            slot = allocatedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    for (queuedVoice = gSynthQueuedVoices; queuedVoice != 0; queuedVoice = queuedVoice->next) {
        if (queuedVoice->handle == resolvedHandle) {
            slot = queuedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    slot = SYNTH_HANDLE_SLOT_INVALID;

found:
    if (slot == SYNTH_HANDLE_SLOT_INVALID) {
        return;
    }

    if ((slot & 0x80000000) == 0) {
        voice = &gSynthVoices[slot];
        state = voice->state;

        if (state == SYNTH_VOICE_STATE_QUEUED) {
            if (voice->prev != 0) {
                voice->prev->next = voice->next;
            } else {
                gSynthQueuedVoices = voice->next;
            }
        } else if (state == SYNTH_VOICE_STATE_ALLOCATED) {
            if (voice->prev != 0) {
                voice->prev->next = voice->next;
            } else {
                gSynthAllocatedVoices = voice->next;
            }

            for (listIndex = 0; listIndex < 2; listIndex++) {
                for (callback = voice->callbackLists[listIndex]; callback != 0; callback = callback->next) {
                    synthCancelCallbackVoices(callback->callbackId);
                }
            }

            for (callback = voice->callbackLists[2]; callback != 0; callback = callback->next) {
                synthCancelCallbackVoices(callback->callbackId);
            }

            synthRecycleVoiceCallbacks(voice);
        }

        if (voice->next != 0) {
            voice->next->prev = voice->prev;
        }

        voice->state = SYNTH_VOICE_STATE_FREE;
        if (gSynthFreeVoices != 0) {
            gSynthFreeVoices->prev = voice;
        }

        voice->next = gSynthFreeVoices;
        voice->prev = 0;
        gSynthFreeVoices = voice;
        return;
    }

    voice = &gSynthVoices[slot & 0x7FFFFFFF];
    if (voice->state != SYNTH_VOICE_STATE_FREE) {
        voice->pendingUpdate.output = 0;
    }
}

void synthSetHandleValue16(u32 handle, u16 value) {
    SynthVoice* allocatedVoice;
    u32 slot;
    SynthVoice* queuedVoice;
    u32 resolvedHandle;
    SynthVoice* voice;

    resolvedHandle = handle & 0x7FFFFFFF;
    for (allocatedVoice = gSynthAllocatedVoices; allocatedVoice != 0; allocatedVoice = allocatedVoice->next) {
        if (allocatedVoice->handle == resolvedHandle) {
            slot = allocatedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    for (queuedVoice = gSynthQueuedVoices; queuedVoice != 0; queuedVoice = queuedVoice->next) {
        if (queuedVoice->handle == resolvedHandle) {
            slot = queuedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    slot = SYNTH_HANDLE_SLOT_INVALID;

found:
    if ((slot & 0x80000000) == 0) {
        voice = &gSynthVoices[slot];
        SYNTH_CHANNEL_VALUE16(voice, 0) = value;
        SYNTH_CHANNEL_VALUE16(voice, 1) = value;
        SYNTH_CHANNEL_VALUE16(voice, 2) = value;
        SYNTH_CHANNEL_VALUE16(voice, 3) = value;
        SYNTH_CHANNEL_VALUE16(voice, 4) = value;
        SYNTH_CHANNEL_VALUE16(voice, 5) = value;
        SYNTH_CHANNEL_VALUE16(voice, 6) = value;
        SYNTH_CHANNEL_VALUE16(voice, 7) = value;
        SYNTH_CHANNEL_VALUE16(voice, 8) = value;
        SYNTH_CHANNEL_VALUE16(voice, 9) = value;
        SYNTH_CHANNEL_VALUE16(voice, 10) = value;
        SYNTH_CHANNEL_VALUE16(voice, 11) = value;
        SYNTH_CHANNEL_VALUE16(voice, 12) = value;
        SYNTH_CHANNEL_VALUE16(voice, 13) = value;
        SYNTH_CHANNEL_VALUE16(voice, 14) = value;
        SYNTH_CHANNEL_VALUE16(voice, 15) = value;
        return;
    }

    voice = &gSynthVoices[slot & 0x7FFFFFFF];
    voice->pendingUpdate.flags |= SYNTH_PENDING_FLAG_VALUE16;
    voice->pendingUpdate.value16 = value;
}

void synthRestoreQueuedHandle(u32 handle) {
    SynthVoice* allocatedVoice;
    u32 resolvedHandle;
    u32 slot;
    SynthVoice* queuedVoice;
    SynthVoice* voice;

    resolvedHandle = handle & 0x7FFFFFFF;
    for (allocatedVoice = gSynthAllocatedVoices; allocatedVoice != 0; allocatedVoice = allocatedVoice->next) {
        if (allocatedVoice->handle == resolvedHandle) {
            slot = allocatedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    for (queuedVoice = gSynthQueuedVoices; queuedVoice != 0; queuedVoice = queuedVoice->next) {
        if (queuedVoice->handle == resolvedHandle) {
            slot = queuedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    slot = SYNTH_HANDLE_SLOT_INVALID;

found:
    if ((slot & 0x80000000) != 0) {
        voice = &gSynthVoices[slot & 0x7FFFFFFF];
        voice->pendingUpdate.flags &= (u8)~SYNTH_PENDING_FLAG_QUEUE;
        return;
    }

    voice = &gSynthVoices[slot];
    if (voice->state != SYNTH_VOICE_STATE_QUEUED) {
        return;
    }

    if (voice->prev != 0) {
        voice->prev->next = voice->next;
    } else {
        gSynthQueuedVoices = voice->next;
    }

    if (voice->next != 0) {
        voice->next->prev = voice->prev;
    }

    voice->next = gSynthAllocatedVoices;
    if (gSynthAllocatedVoices != 0) {
        gSynthAllocatedVoices->prev = voice;
    }

    voice->prev = 0;
    gSynthAllocatedVoices = voice;
    voice->state = SYNTH_VOICE_STATE_ALLOCATED;
}

void synthSetHandleMixData(u32 handle, u32 value0, u32 value1) {
    SynthVoice* allocatedVoice;
    u32 resolvedHandle;
    u32 slot;
    SynthVoice* queuedVoice;
    SynthVoice* voice;

    resolvedHandle = handle & 0x7FFFFFFF;
    for (allocatedVoice = gSynthAllocatedVoices; allocatedVoice != 0; allocatedVoice = allocatedVoice->next) {
        if (allocatedVoice->handle == resolvedHandle) {
            slot = allocatedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    for (queuedVoice = gSynthQueuedVoices; queuedVoice != 0; queuedVoice = queuedVoice->next) {
        if (queuedVoice->handle == resolvedHandle) {
            slot = queuedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    slot = SYNTH_HANDLE_SLOT_INVALID;

found:
    if (slot == SYNTH_HANDLE_SLOT_INVALID) {
        return;
    }

    if ((slot & 0x80000000) == 0) {
        voice = &gSynthVoices[slot];
        voice->immediateMixValue0 = value0;
        voice->immediateMixValue1 = value1;
        return;
    }

    voice = &gSynthVoices[slot & 0x7FFFFFFF];
    voice->pendingUpdate.flags |= SYNTH_PENDING_FLAG_MIX_DATA;
    voice->pendingUpdate.mixValue0 = value0;
    voice->pendingUpdate.mixValue1 = value1;
}

void synthUpdateHandle(u32 value0, u32 value1, u32 handle, u8 mode) {
    u8* controllerStudioMapCompare;
    u8* controllerStudioMapValue;
    u8 modeType;
    u8 studio;
    u32 resolvedHandle;
    u32 i;
    u32 slot;
    SynthVoice* allocatedVoice;
    SynthVoice* queuedVoice;
    SynthVoice* voice;

    resolvedHandle = handle & 0x7FFFFFFF;
    for (allocatedVoice = gSynthAllocatedVoices; allocatedVoice != 0; allocatedVoice = allocatedVoice->next) {
        if (allocatedVoice->handle == resolvedHandle) {
            slot = allocatedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    for (queuedVoice = gSynthQueuedVoices; queuedVoice != 0; queuedVoice = queuedVoice->next) {
        if (queuedVoice->handle == resolvedHandle) {
            slot = queuedVoice->slotIndex | (handle & 0x80000000);
            goto found;
        }
    }

    slot = SYNTH_HANDLE_SLOT_INVALID;

found:
    if (slot == SYNTH_HANDLE_SLOT_INVALID) {
        return;
    }

    if ((slot & 0x80000000) == 0) {
        voice = &gSynthVoices[slot];
        controllerStudioMapCompare = voice->studioMap;
        controllerStudioMapValue = voice->studioMap;

        synthSetFade(value0, value1, voice->currentStudio, mode, handle);

        for (i = 0; i < 0x40; i++) {
            if (*controllerStudioMapCompare != voice->currentStudio) {
                synthSetFade(value0, value1, *controllerStudioMapValue, 0, 0xFFFFFFFF);
            }
            controllerStudioMapCompare++;
            controllerStudioMapValue++;
        }
        return;
    }

    voice = &gSynthVoices[slot & 0x7FFFFFFF];
    modeType = mode & 0xF;
    studio = (u8)value0;

    if (modeType == 2) {
        voice->pendingUpdate.flags |= SYNTH_PENDING_FLAG_QUEUE;
        voice->pendingUpdate.studio = studio;
        return;
    }

    if (modeType == 0) {
        voice->pendingUpdate.studio = studio;
        return;
    }

    if (modeType == 1) {
        voice->pendingUpdate.output = 0;
        return;
    }

    if (modeType < 4) {
        voice->pendingUpdate.flags |= SYNTH_PENDING_FLAG_UPDATE_MODE3;
        voice->pendingUpdate.studio = studio;
    }
}
