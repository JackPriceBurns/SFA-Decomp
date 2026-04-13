#include "src/audio/synth_internal.h"

typedef short s16;

#define SYNTH_TRACK_COMMAND_END 0xFFFF
#define SYNTH_TRACK_COMMAND_JUMP 0xFFFE

#define SYNTH_CHANNEL_EVENT(voice, channel) ((SynthSequenceEvent*)&(voice)->unkEE0[0x4 + ((channel) * 0x18)])
#define SYNTH_KEYGROUP_MAP(voice) (*(u8**)&(voice)->unkEE0[0x604])
#define SYNTH_KEYGROUP_STATE(voice, index) ((SynthKeyGroupState*)&(voice)->unkEE0[0x608 + ((index) * 0x38)])
#define SYNTH_SEQUENCE_STATE(voice, channel) ((SynthSequenceState*)&(voice)->unk364[(channel) * 0x2C])
#define SYNTH_TRACK_CURSOR(voice, channel) ((SynthTrackCursor*)&(voice)->unk124[(channel) * 8])

typedef struct SynthTrackCursor {
    u8* base;
    void* current;
} SynthTrackCursor;

typedef struct SynthSequenceEvent {
    struct SynthSequenceEvent* next;
    struct SynthSequenceEvent* prev;
    u32 value;
    void* eventData;
    void* state;
    u8 type;
    u8 channel;
    u8 pad16[2];
} SynthSequenceEvent;

typedef struct SynthSequenceState {
    u32 currentValue;
    u32 valueOffset;
    u8* stream;
    void* eventData;
    u8* primaryStream;
    u16 primaryValue;
    s16 primaryStep;
    u32 primaryLimit;
    u8* secondaryStream;
    u16 secondaryValue;
    s16 secondaryStep;
    u32 secondaryLimit;
    u8 controller;
    u8 pad29[3];
} SynthSequenceState;

typedef struct SynthSequenceQueue {
    u8 unk00[0x1C];
    SynthSequenceEvent* eventList;
} SynthSequenceQueue;

typedef struct SynthTrackCommand {
    u32 value0;
    u32 value1;
    u16 command;
    u16 arg;
} SynthTrackCommand;

typedef struct SynthKeyGroupState {
    u8 unk00[0x36];
    u8 active;
    u8 pad37;
} SynthKeyGroupState;

u8* synthReadVariablePair(u8* input, u16* value0, s16* value1) {
    u8 high;
    u8 low;

    high = input[0];
    low = input[1];
    if (high == 0x80 && low == 0) {
        return 0;
    }

    if ((high & 0x80) != 0) {
        *value0 = (u16)(((high & 0x7F) << 8) | low);
        input += 2;
    } else {
        *value0 = high;
        input += 1;
    }

    high = input[0];
    low = input[1];
    if ((high & 0x80) != 0) {
        *value1 = (s16)(((s16)(((high & 0x7F) << 8) | low) << 1) >> 1);
        input += 2;
    } else {
        *value1 = (s16)(((s16)high << 9) >> 9);
        input += 1;
    }

    return input;
}

SynthSequenceEvent* synthGetNextChannelEvent(u8 channel) {
    SynthSequenceEvent* event;
    SynthKeyGroupState* keyGroupState;
    SynthSequenceState* state;
    SynthTrackCommand* command;
    SynthTrackCursor* cursor;
    SynthVoice* voice;
    u8* keyGroupMap;
    u8* stream;
    u32 value;

    voice = gSynthCurrentVoice;
    cursor = SYNTH_TRACK_CURSOR(voice, channel);
    if (cursor->current == 0) {
        return 0;
    }

    state = SYNTH_SEQUENCE_STATE(voice, channel);
    event = SYNTH_CHANNEL_EVENT(voice, channel);
    event->channel = channel;
    event->state = state;

    if (state->stream != 0) {
        while (1) {
            stream = state->stream;
            value = *(u16*)stream + state->currentValue;
            if (value < state->primaryLimit) {
                if (value < state->secondaryLimit) {
                    if (stream[2] == 0xFF && stream[3] == 0xFF) {
                        state->stream = 0;
                        break;
                    }

                    event->eventData = stream;
                    state->currentValue = value;
                    if ((stream[2] & 0x80) != 0) {
                        state->stream = stream + 4;
                    } else if ((stream[2] | stream[3]) == 0) {
                        state->stream = stream + 4;
                        continue;
                    } else {
                        state->stream = stream + 6;
                    }

                    event->type = 0;
                    event->value = value + state->valueOffset;
                    return event;
                }
            } else if (state->primaryLimit < state->secondaryLimit) {
                event->value = state->primaryLimit + state->valueOffset;
                event->type = 2;
                return event;
            }

            event->value = state->secondaryLimit + state->valueOffset;
            event->type = 1;
            return event;
        }
    }

    command = cursor->current;
    if (command->command == SYNTH_TRACK_COMMAND_END) {
        cursor->current = 0;
        return 0;
    }

    if (command->command == SYNTH_TRACK_COMMAND_JUMP) {
        keyGroupMap = SYNTH_KEYGROUP_MAP(voice);
        if (keyGroupMap == 0) {
            keyGroupState = SYNTH_KEYGROUP_STATE(voice, 0);
            if (keyGroupState->active != 0) {
                cursor->current = 0;
                return 0;
            }
        } else {
            keyGroupState = SYNTH_KEYGROUP_STATE(voice, keyGroupMap[channel]);
            if (keyGroupState->active != 0) {
                cursor->current = 0;
                return 0;
            }
        }

        event->type = 3;
        event->value = command->value0;
        cursor->current = cursor->base + (command->arg * sizeof(SynthTrackCommand));
        return event;
    }

    event->type = 4;
    event->value = command->value0;
    event->eventData = command;
    cursor->current = command + 1;
    return event;
}

void synthInsertChannelEvent(SynthSequenceQueue* queue, SynthSequenceEvent* event) {
    SynthSequenceEvent* current;
    SynthSequenceEvent* prev;

    prev = 0;
    for (current = queue->eventList; current != 0; current = current->next) {
        if (current->value > event->value) {
            event->next = current;
            event->prev = prev;
            if (prev != 0) {
                prev->next = event;
            } else {
                queue->eventList = event;
            }
            current->prev = event;
            return;
        }

        prev = current;
    }

    event->prev = prev;
    if (prev != 0) {
        prev->next = event;
    } else {
        queue->eventList = event;
    }
    event->next = 0;
}
