/* TODO: restore stripped imported address metadata if needed. */

#include "PowerPC_EABI_Support/Runtime/NMWException.h"
#include "PowerPC_EABI_Support/Runtime/MWCPlusLib.h"

DestructorChain* __global_destructor_chain;

void __destroy_global_chain(void) {
    DestructorChain* iter;
    while ((iter = __global_destructor_chain) != 0) {
        __global_destructor_chain = iter->next;
        DTORCALL_COMPLETE(iter->destructor, iter->object);
    }
}

void* __register_global_object(void* object, void* destructor, void* regmem) {
    ((DestructorChain*)regmem)->next = __global_destructor_chain;
    ((DestructorChain*)regmem)->destructor = destructor;
    ((DestructorChain*)regmem)->object = object;
    __global_destructor_chain = (DestructorChain*)regmem;
    return object;
}

/* clang-format off */
static __declspec(section ".dtors") void* const __destroy_global_chain_reference = __destroy_global_chain;
/* clang-format on */
