#include <stdlib.h>

/******************************************************************************
 *                             Event Loop Ptr                                 *
 ******************************************************************************/
typedef struct __UdpSocket __UdpSocketRaw;

void __UdpSocketRaw_drop(__UdpSocketRaw **evlp);
static inline __UdpSocketRaw *__UdpSocketRaw_clone(__UdpSocketRaw *evlp) {
  return evlp;
}

#define i_type __UdpSocketPtr
#define i_key __UdpSocketRaw *
#define i_keydrop __UdpSocketRaw_drop
#define i_keyclone __UdpSocketRaw_clone
#include "stc/arc.h"
