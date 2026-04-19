/* TODO: restore stripped imported address metadata if needed. */

#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/support.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/MWTrace.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"
#include <string.h>

DSError TRKSuppAccessFile(u32 file_handle, u8* data, size_t* count, DSIOResult* io_result,
                          BOOL need_reply, BOOL read) {
    DSError error;
    int replyBufferId;
    TRKBuffer* replyBuffer;
    u32 length;
    int bufferId;
    TRKBuffer* buffer;
    u32 i;
    u8 replyIOResult;
    u32 replyLength;
    BOOL exit;
    CommandReply reply;

    if (data == NULL || *count == 0) {
        return DS_ParameterError;
    }

    exit = FALSE;
    *io_result = DS_IONoError;
    i = 0;
    error = DS_NoError;
    while (!exit && i < *count && error == DS_NoError && *io_result == 0) {
        memset(&reply, 0, sizeof(CommandReply));

        if (*count - i <= 0x800) {
            length = *count - i;
        } else {
            length = 0x800;
        }

        reply.commandID.b = read ? DSMSG_ReadFile : DSMSG_WriteFile;

        if (read) {
            reply._00 = 0x40;
        } else {
            reply._00 = length + 0x40;
        }

        reply.replyError.r = file_handle;
        *(u16*)&reply._0C = length;

        TRKGetFreeBuffer(&bufferId, &buffer);
        error = TRKAppendBuffer_ui8(buffer, (u8*)&reply, 0x40);

        if (!read && error == DS_NoError) {
            error = TRKAppendBuffer_ui8(buffer, data + i, length);
        }

        if (error == DS_NoError) {
            if (need_reply) {
                BOOL b = read && file_handle == 0;

                error = TRKRequestSend(buffer, &replyBufferId, read ? 5 : 5, 3, !b);
                if (error == DS_NoError) {
                    replyBuffer = (TRKBuffer*)TRKGetBuffer(replyBufferId);
                }
                replyIOResult = *(u32*)(replyBuffer->data + 0x10);
                replyLength = *(u16*)(replyBuffer->data + 0x14);
                if (read && error == DS_NoError && replyLength <= length) {
                    TRKSetBufferPosition(replyBuffer, 0x40);
                    error = TRKReadBuffer_ui8(replyBuffer, data + i, replyLength);
                    if (error == DS_MessageBufferReadError) {
                        error = DS_NoError;
                    }
                }

                if (replyLength != length) {
                    length = replyLength;
                    exit = TRUE;
                }

                *io_result = (DSIOResult)replyIOResult;
                TRKReleaseBuffer(replyBufferId);
            } else {
                error = TRKMessageSend(buffer);
            }
        }

        TRKReleaseBuffer(bufferId);
        i += length;
    }

    *count = i;
    return error;
}

DSError TRKRequestSend(TRKBuffer* msgBuf, int* bufferId, u32 p1, u32 p2, int p3) {
    int error = DS_NoError;
    TRKBuffer* buffer;
    u32 counter;
    int count;
    u8 msgCmd;
    int msgReplyError;
    BOOL badReply = TRUE;

    *bufferId = -1;

    for (count = p2 + 1; count != 0 && *bufferId == -1 && error == DS_NoError; count--) {
        MWTRACE(1, "Calling MessageSend\n");
        error = TRKMessageSend(msgBuf);
        if (error == DS_NoError) {
            if (p3) {
                counter = 0;
            }

            while (TRUE) {
                do {
                    *bufferId = TRKTestForPacket();
                    if (*bufferId != -1)
                        break;
                } while (!p3 || ++counter < 79999980);

                if (*bufferId == -1)
                    break;

                badReply = 0;

                buffer = TRKGetBuffer(*bufferId);
                TRKSetBufferPosition(buffer, 0);
                OutputData(&buffer->data[0], buffer->length);
                msgCmd = buffer->data[4];
                MWTRACE(1, "msg_command : 0x%02x hdr->cmdID 0x%02x\n", msgCmd, msgCmd);

                if (msgCmd >= DSMSG_ReplyACK)
                    break;

                TRKProcessInput(*bufferId);
                *bufferId = -1;
            }

            if (*bufferId != -1) {
                if (buffer->length < p1) {
                    badReply = TRUE;
                }
                if (error == DS_NoError && !badReply) {
                    msgReplyError = buffer->data[8];
                    MWTRACE(1, "msg_error : 0x%02x\n", msgReplyError);
                }
                if (error == DS_NoError && !badReply) {
                    if ((int)msgCmd != DSMSG_ReplyACK || msgReplyError != DSREPLY_NoError) {
                        MWTRACE(8,
                                "RequestSend : Bad ack or non ack received msg_command : 0x%02x "
                                "msg_error 0x%02x\n",
                                msgCmd, msgReplyError);
                        badReply = TRUE;
                    }
                }
                if (error != DS_NoError || badReply) {
                    TRKReleaseBuffer(*bufferId);
                    *bufferId = -1;
                }
            }
        }
    }

    if (*bufferId == -1) {
        error = DS_Error800;
    }

    return error;
}

DSError HandleOpenFileSupportRequest(const char* path, u8 replyError, u32* param_3,
                                     DSIOResult* ioResult) {
    DSError error;
    int replyBufferId;
    int bufferId;
    TRKBuffer* replyBuffer;
    TRKBuffer* buffer;
    u16 pathLength;

    *param_3 = 0;
    error = TRKGetFreeBuffer(&bufferId, &buffer);

    if (error == DS_NoError) {
        error = TRKAppendBuffer1_ui8(buffer, DSMSG_OpenFile);
    }

    if (error == DS_NoError) {
        error = TRKAppendBuffer1_ui8(buffer, replyError);
    }

    if (error == DS_NoError) {
        pathLength = strlen(path) + 1;
        error = TRKAppendBuffer1_ui16(buffer, pathLength);
    }

    if (error == DS_NoError) {
        error = TRKAppendBuffer_ui8(buffer, (u8*)path, pathLength);
    }

    if (error == DS_NoError) {
        *ioResult = DS_IONoError;
        error = TRKRequestSend(buffer, &replyBufferId, 7, 3, 0);

        if (error == DS_NoError) {
            replyBuffer = TRKGetBuffer(replyBufferId);
            TRKSetBufferPosition(replyBuffer, 2);
        }

        if (error == DS_NoError) {
            error = TRKReadBuffer1_ui8(replyBuffer, (u8*)ioResult);
        }

        if (error == DS_NoError) {
            error = TRKReadBuffer1_ui32(replyBuffer, param_3);
        }

        TRKReleaseBuffer(replyBufferId);
    }

    TRKReleaseBuffer(bufferId);
    return error;
}

DSError HandleCloseFileSupportRequest(int replyError, DSIOResult* ioResult) {
    DSError error;
    int replyBufferId;
    int bufferId;
    TRKBuffer* buffer;
    TRKBuffer* replyBuffer;

    error = TRKGetFreeBuffer(&bufferId, &buffer);

    if (error == DS_NoError) {
        error = TRKAppendBuffer1_ui8(buffer, DSMSG_CloseFile);
    }

    if (error == DS_NoError) {
        error = TRKAppendBuffer1_ui32(buffer, replyError);
    }

    if (error == DS_NoError) {
        *ioResult = DS_IONoError;
        error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);

        if (error == DS_NoError) {
            replyBuffer = TRKGetBuffer(replyBufferId);
            TRKSetBufferPosition(replyBuffer, 2);
        }

        if (error == DS_NoError) {
            error = TRKReadBuffer1_ui8(replyBuffer, (u8*)ioResult);
        }

        TRKReleaseBuffer(replyBufferId);
    }

    TRKReleaseBuffer(bufferId);
    return error;
}

DSError HandlePositionFileSupportRequest(DSReplyError replyErr, u32 param_2, u8 param_3,
                                         DSIOResult* ioResult) {
    DSError error;
    int replyBufferId;
    int bufferId;
    TRKBuffer* buffer;
    TRKBuffer* replyBuffer;

    error = TRKGetFreeBuffer(&bufferId, &buffer);

    if (error == DS_NoError) {
        error = TRKAppendBuffer1_ui8(buffer, DSMSG_PositionFile);
    }

    if (error == DS_NoError) {
        error = TRKAppendBuffer1_ui32(buffer, replyErr);
    }

    if (error == DS_NoError) {
        error = TRKAppendBuffer1_ui32(buffer, param_2);
    }

    if (error == DS_NoError) {
        error = TRKAppendBuffer1_ui8(buffer, param_3);
    }

    if (error == DS_NoError) {
        *ioResult = DS_IONoError;
        error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);

        if (error == DS_NoError) {
            replyBuffer = TRKGetBuffer(replyBufferId);
            TRKSetBufferPosition(replyBuffer, 2);
        }

        if (error == DS_NoError) {
            error = TRKReadBuffer1_ui8(replyBuffer, (u8*)ioResult);
        }

        TRKReleaseBuffer(replyBufferId);
    }

    TRKReleaseBuffer(bufferId);
    return error;
}
