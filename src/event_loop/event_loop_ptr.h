/******************************************************************************
 *                             Event Loop Ptr                                 *
 ******************************************************************************/
typedef struct __EventLoop *__EventLoopPtr;

void __event_loop_ptr_destroy(__EventLoopPtr *evlp);
__EventLoopPtr __event_loop_ptr_clone(__EventLoopPtr evlp);

#define i_type event_loop_ptr
#define i_key __EventLoopPtr
#define i_keydrop __event_loop_ptr_destroy
#define i_keyclone __event_loop_ptr_clone
#include "stc/arc.h"

typedef struct event_loop_ptr event_loop_ptr_t;
