/******************************************************************************
 *                             Udp Socket Ptr                                 *
 ******************************************************************************/
typedef struct __UdpSocket *__UdpSocketPtr;

void __udp_socket_ptr_destroy(__UdpSocketPtr *udp_ptr);
__UdpSocketPtr __udp_socket_ptr_clone(__UdpSocketPtr udp);

#define i_type udp_socket_ptr
#define i_key __UdpSocketPtr
#define i_keydrop __udp_socket_ptr_destroy
#define i_keyclone __udp_socket_ptr_clone
#include "stc/arc.h"

typedef struct udp_socket_ptr udp_socket_ptr_t;
