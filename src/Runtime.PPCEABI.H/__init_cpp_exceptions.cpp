extern "C" int __register_fragment(void*, void*);
extern "C" void __unregister_fragment(int);
extern "C" void __destroy_global_chain(void);
extern "C" char _eti_init_info[];

asm void* GetR2(void) {
    mr r3, r2
    blr
}

static int fragmentID = -2;

extern "C" void __fini_cpp_exceptions(void) {
    if (fragmentID != -2) {
        __unregister_fragment(fragmentID);
        fragmentID = -2;
    }
}

extern "C" void __init_cpp_exceptions(void) {
    if (fragmentID == -2) {
        fragmentID = __register_fragment(_eti_init_info, GetR2());
    }
}

__declspec(section ".ctors") void (*const __init_cpp_exceptions_reference)(void) =
    __init_cpp_exceptions;

__declspec(section ".dtors") void (*const __destroy_global_chain_reference)(void) =
    __destroy_global_chain;
__declspec(section ".dtors") void (*const __fini_cpp_exceptions_reference)(void) =
    __fini_cpp_exceptions;
