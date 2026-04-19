#include "dolphin/dvd/__dvd.h"

static void (*FatalFunc)(void);

void __DVDPrintFatalMessage(void) {
    if (FatalFunc != NULL) {
        FatalFunc();
    }
}
