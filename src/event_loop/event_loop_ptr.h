#include <stdlib.h>

/******************************************************************************
 *                             Event Loop Ptr                                 *
 ******************************************************************************/
typedef struct __EventLoop __EventLoopRaw;

static inline void __EventLoopRaw_drop(__EventLoopRaw **evlp) { free(*evlp); }
static inline __EventLoopRaw *__EventLoopRaw_clone(__EventLoopRaw *evlp) {
  return evlp;
}

#define i_type __EventLoopPtr
#define i_key __EventLoopRaw *
#define i_keydrop __EventLoopRaw_drop
#define i_keyclone __EventLoopRaw_clone
#include "stc/arc.h"
