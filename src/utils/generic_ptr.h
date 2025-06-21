/*
Each module create it's personalized types as pointers.
 Like Event loop is represented by EventLoopPtr. But sometimes
 we need to pass this personalized ptrs to sam callback or handler
 wich does not know the type under the pointer. That's why we need
 generic ptrs.
*/

void __GenericPtrValue_destroy(void **udp_socket) { return; };

void *__GenericPtrValue_clone(void *udp_socket) { return udp_socket; };

#define i_type GenericPtr
#define i_key void *
#define i_keydrop __GenericPtrValue_destroy
#define i_keyclone __GenericPtrValue_clone
#include "stc/arc.h"
