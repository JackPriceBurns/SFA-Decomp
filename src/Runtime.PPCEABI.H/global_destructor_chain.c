typedef void (*DestructorFunc)(void*, int);

typedef struct DestructorChain {
    struct DestructorChain* next;
    DestructorFunc destructor;
    void* object;
} DestructorChain;

extern DestructorChain* __global_destructor_chain;

void __destroy_global_chain(void) {
    DestructorChain* chain;

    while ((chain = __global_destructor_chain) != (void*)0) {
        __global_destructor_chain = chain->next;
        chain->destructor(chain->object, -1);
    }
}

__declspec(section ".dtors") static void (*const sDestroyGlobalChainReference)(void) =
    __destroy_global_chain;
