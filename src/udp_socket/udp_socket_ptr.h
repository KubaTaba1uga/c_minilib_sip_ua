#include <stdlib.h>

/******************************************************************************
 *                             Udp Socket Ptr                                 *
 ******************************************************************************/
typedef struct __UdpSocket __UdpSocket;

void __UdpSocket_destroy(__UdpSocket **evlp);
__UdpSocket *__UdpSocket_clone(__UdpSocket *evlp);

#define i_type __UdpSocketPtr
#define i_key __UdpSocket *
#define i_keydrop __UdpSocket_drop
#define i_keyclone __UdpSocket_clone
#include "stc/arc.h"
