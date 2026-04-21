#include "PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/abort_exit.h"
#include "PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/critical_regions.h"
#include "PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/signal.h"
#include "Runtime.PPCEABI.H/NMWException.h"
#include "stddef.h"

void _ExitProcess();

extern void (*_dtors[])(void);

static void (*__atexit_funcs_803DB718[64])(void);

static void (*__console_exit)(void);

void (*__stdio_exit)(void);

static int __atexit_curr_func_803DF074;

static int __aborting;

void exit(int status) {
    void (**dtor)(void);

    if (!__aborting) {
        __destroy_global_chain();

        dtor = _dtors;
        while (*dtor != NULL) {
            (*dtor)();
            dtor++;
        }

        if (__stdio_exit != NULL) {
            __stdio_exit();
            __stdio_exit = NULL;
        }
    }

    while (__atexit_curr_func_803DF074 > 0)
        __atexit_funcs_803DB718[--__atexit_curr_func_803DF074]();

    if (__console_exit != NULL) {
        __console_exit();
        __console_exit = NULL;
    }

    _ExitProcess();
}
