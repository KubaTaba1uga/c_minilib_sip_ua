#include <stdlib.h>

/******************************************************************************
 *                             Event Loop Ptr                                 *
 ******************************************************************************/
typedef struct __EventLoop __EventLoop;

static inline void __EventLoop_drop(struct __EventLoop **evlp) { free(*evlp); }
static inline struct __EventLoop *__EventLoop_clone(struct __EventLoop *evlp) {
  return evlp;
}

#define i_type __EventLoopPtr
#define i_key __EventLoop *
#define i_keydrop __EventLoop_drop
#define i_keyclone __EventLoop_clone
#include "stc/arc.h"
