/* TODO: restore stripped imported address metadata if needed. */

#include "PowerPC_EABI_Support/MetroTRK/trk.h"

UARTError WriteUART1(u8 byte);
UARTError WriteUARTFlush(void);

DSError TRKMessageSend(TRKBuffer* msg) {
    UARTError error;
    u32 i;
    u8 checksum;
    u8 byte;

    checksum = 0;
    for (i = 0; i < msg->length; i++) {
        checksum += msg->data[i];
    }
    checksum ^= 0xFF;

    error = WriteUART1(0x7E);
    if (error == UART_NoError) {
        for (i = 0; i < msg->length; i++) {
            byte = msg->data[i];
            if (byte == 0x7E || byte == 0x7D) {
                error = WriteUART1(0x7D);
                byte ^= 0x20;
                if (error != UART_NoError) {
                    break;
                }
            }

            error = WriteUART1(byte);
            if (error != UART_NoError) {
                break;
            }
        }
    }

    if (error == UART_NoError) {
        byte = checksum;
        if (byte == 0x7E || byte == 0x7D) {
            error = WriteUART1(0x7D);
            byte ^= 0x20;
        }

        if (error == UART_NoError) {
            error = WriteUART1(byte);
        }
    }

    if (error == UART_NoError) {
        error = WriteUART1(0x7E);
    }

    if (error == UART_NoError) {
        error = WriteUARTFlush();
    }

    return error;
}
