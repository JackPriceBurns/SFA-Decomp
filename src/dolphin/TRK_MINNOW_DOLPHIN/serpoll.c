/* TODO: restore stripped imported address metadata if needed. */

#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/serpoll.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/nubevent.h"
#include "PowerPC_EABI_Support/MetroTRK/trk.h"

static TRKFramingState gTRKFramingState;

void* gTRKInputPendingPtr;

static inline BOOL serpoll_inline_00(TRKBuffer* buffer) {
    if (buffer->length < 2) {
        TRKStandardACK(buffer, DSMSG_ReplyNAK, DSREPLY_PacketSizeError);
        if (gTRKFramingState.msgBufID != -1) {
            TRKReleaseBuffer(gTRKFramingState.msgBufID);
            gTRKFramingState.msgBufID = -1;
        }
        gTRKFramingState.buffer = NULL;
        gTRKFramingState.receiveState = DSRECV_Wait;
        return FALSE;
    }

    buffer->position = 0;
    buffer->length--;
    return TRUE;
}

DSError TRKTerminateSerialHandler(void) {
    return DS_NoError;
}

DSError TRKInitializeSerialHandler(void) {
    gTRKFramingState.msgBufID = -1;
    gTRKFramingState.receiveState = DSRECV_Wait;
    gTRKFramingState.isEscape = FALSE;

    return DS_NoError;
}

void TRKProcessInput(int bufferIdx) {
    TRKEvent event;

    TRKConstructEvent(&event, NUBEVENT_Request);
    gTRKFramingState.msgBufID = -1;
    event.msgBufID = bufferIdx;
    TRKPostEvent(&event);
}

void TRKGetInput(void) {
    TRKBuffer* msgBuffer;
    int id;
    u8 command;

    id = TRKTestForPacket();
    if (id == -1) {
        return;
    }

    msgBuffer = TRKGetBuffer(id);
    TRKSetBufferPosition(msgBuffer, 0);
    TRKReadBuffer1_ui8(msgBuffer, &command);
    if (command < DSMSG_ReplyACK) {
        TRKProcessInput(id);
    } else {
        TRKReleaseBuffer(id);
    }
}

MessageBufferID TRKTestForPacket(void) {
    s32 result;
    s32 err;
    u8 c;
    s32 msgBufID;

    result = 0;
    err = TRKReadUARTPoll(&c);
    while (err == 0 && result == 0) {
        if (gTRKFramingState.receiveState != DSRECV_InFrame) {
            gTRKFramingState.isEscape = FALSE;
        }

        switch (gTRKFramingState.receiveState) {
            case DSRECV_Wait:
                if (c == 0x7E) {
                    result = TRKGetFreeBuffer(&gTRKFramingState.msgBufID, &gTRKFramingState.buffer);
                    gTRKFramingState.fcsType = 0;
                    gTRKFramingState.receiveState = DSRECV_Found;
                }
                break;

            case DSRECV_Found:
                if (c == 0x7E) {
                    break;
                }
                gTRKFramingState.receiveState = DSRECV_InFrame;

            case DSRECV_InFrame:
                if (c == 0x7E) {
                    if (gTRKFramingState.isEscape) {
                        TRKStandardACK(gTRKFramingState.buffer, DSMSG_ReplyNAK, DSREPLY_EscapeError);
                        if (gTRKFramingState.msgBufID != -1) {
                            TRKReleaseBuffer(gTRKFramingState.msgBufID);
                            gTRKFramingState.msgBufID = -1;
                        }
                        gTRKFramingState.buffer = NULL;
                        gTRKFramingState.receiveState = DSRECV_Wait;
                        break;
                    }

                    if (serpoll_inline_00(gTRKFramingState.buffer)) {
                        msgBufID = gTRKFramingState.msgBufID;
                        gTRKFramingState.msgBufID = -1;
                        gTRKFramingState.buffer = NULL;
                        gTRKFramingState.receiveState = DSRECV_Wait;
                        return msgBufID;
                    }

                    gTRKFramingState.receiveState = DSRECV_Wait;
                } else {
                    if (gTRKFramingState.isEscape) {
                        c ^= 0x20;
                        gTRKFramingState.isEscape = FALSE;
                    } else if (c == 0x7D) {
                        gTRKFramingState.isEscape = TRUE;
                        break;
                    }

                    result = TRKAppendBuffer1_ui8(gTRKFramingState.buffer, c);
                    gTRKFramingState.fcsType += c;
                }
                break;

            case DSRECV_FrameOverflow:
                if (c == 0x7E) {
                    if (gTRKFramingState.msgBufID != -1) {
                        TRKReleaseBuffer(gTRKFramingState.msgBufID);
                        gTRKFramingState.msgBufID = -1;
                    }
                    gTRKFramingState.buffer = NULL;
                    gTRKFramingState.receiveState = DSRECV_Wait;
                }
                break;
        }

        err = TRKReadUARTPoll(&c);
    }

    return -1;
}
