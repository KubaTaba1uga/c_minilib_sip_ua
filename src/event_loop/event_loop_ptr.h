#include <stdlib.h>

/******************************************************************************
 *                             Event Loop Ptr                                 *
 ******************************************************************************/
typedef struct __EventLoop __EventLoop;

void __EventLoop_destroy(__EventLoop **evlp);
__EventLoop *__EventLoop_clone(__EventLoop *evlp);

#define i_type __EventLoopPtr
#define i_key __EventLoop *
#define i_keydrop __EventLoop_destroy
#define i_keyclone __EventLoop_clone
#include "stc/arc.h"
