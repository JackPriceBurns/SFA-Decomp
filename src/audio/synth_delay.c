#include "src/audio/synth_internal.h"

typedef void (*SynthDelayCallback)(void);

#define SYNTH_DELAY_ACTION_FREE 1
#define SYNTH_DELAY_ACTION_QUEUE 2
#define SYNTH_DELAY_ACTION_CLEAR_MIX 3

void synthInsertDelayedNode(SynthDelayedEntry* entry, s32 nodeIndex, u32 delay) {
    SynthDelayedNode** bucketHead;
    SynthDelayedNode** bucketHeads;
    SynthDelayedNode** head;
    SynthDelayedNode* node;
    u8 targetBucket;

    targetBucket = (u8)(((delay >> 8) + gSynthDelayBucketCursor) & 0x1F);
    bucketHeads = gSynthDelayStorage.bucketHeads[targetBucket];

    if (nodeIndex == 1) {
        node = &entry->nodes[1];
        if (node->bucketIndex != SYNTH_DELAY_BUCKET_INVALID) {
            if (node->bucketIndex == targetBucket) {
                return;
            }

            if (node->next != 0) {
                node->next->prev = node->prev;
            }

            if (node->prev == 0) {
                bucketHead = gSynthDelayStorage.bucketHeads[node->bucketIndex];
                bucketHead[2] = node->next;
            } else {
                node->prev->next = node->next;
            }
        }

        head = &bucketHeads[2];
    } else if (nodeIndex < 1) {
        if (nodeIndex < 0) {
            return;
        }

        node = &entry->nodes[0];
        if (node->bucketIndex != SYNTH_DELAY_BUCKET_INVALID) {
            if (node->bucketIndex == targetBucket) {
                return;
            }

            if (node->next != 0) {
                node->next->prev = node->prev;
            }

            if (node->prev == 0) {
                bucketHead = gSynthDelayStorage.bucketHeads[node->bucketIndex];
                bucketHead[0] = node->next;
            } else {
                node->prev->next = node->next;
            }
        }

        head = &bucketHeads[0];
    } else if (nodeIndex < 3) {
        node = &entry->nodes[2];
        if (node->bucketIndex != SYNTH_DELAY_BUCKET_INVALID) {
            return;
        }

        head = &bucketHeads[1];
    } else {
        return;
    }

    node->bucketIndex = targetBucket;
    node->next = *head;

    if (*head != 0) {
        (*head)->prev = node;
    }

    node->prev = 0;
    *head = node;
}

void synthInitDelayedEntry(SynthDelayedEntry* entry) {
    entry->word0 = gSynthDelayedActionWord0;
    entry->word1 = gSynthDelayedActionWord1;
    entry->word2.word = gSynthDelayedActionWord0;
    entry->word3 = gSynthDelayedActionWord1;

    synthInsertDelayedNode(entry, 0, 0);
    synthInsertDelayedNode(entry, 1, 0);
}

void synthRequeueDelayedEntry(SynthDelayedEntry* entry) {
    synthInsertDelayedNode(entry, 0, 0);
    synthInsertDelayedNode(entry, 1, 0);
}

void synthQueueDelayedAction(SynthDelayedEntry* entry) {
    synthInsertDelayedNode(entry, 2, 0);
}

void synthFlushDelayedBucket(SynthDelayedNode** head, SynthDelayCallback callback) {
    SynthDelayedNode* next;
    SynthDelayedNode* node;

    node = *head;
    while (node != 0) {
        next = node->next;
        node->bucketIndex = 0xFF;

        if (gSynthVoiceSlots[node->voiceIndex].callbackActive == 0) {
            callback();
        }

        node = next;
    }

    *head = 0;
}

void synthDispatchDelayedAction(SynthFade* fade) {
    if (fade->delayAction == SYNTH_DELAY_ACTION_QUEUE) {
        synthQueueHandle(fade->handle);
        return;
    }

    if (fade->delayAction < SYNTH_DELAY_ACTION_QUEUE) {
        if (fade->delayAction == SYNTH_DELAY_ACTION_FREE) {
            synthFreeHandle(fade->handle);
        }
        return;
    }

    if (fade->delayAction < 4) {
        synthSetHandleMixData(fade->handle, 0, 0);
    }
}
