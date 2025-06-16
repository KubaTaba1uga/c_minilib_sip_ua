#include <stdlib.h>

/******************************************************************************
 *                             Sip Transport Ptr                              *
 ******************************************************************************/
typedef struct __SipTransport __SipTransport;

void __SipTransport_destroy(__SipTransport **evlp);
__SipTransport *__SipTransport_clone(__SipTransport *evlp);

#define i_type __SipTransportPtr
#define i_key __SipTransport *
#define i_keydrop __SipTransport_destroy
#define i_keyclone __SipTransport_clone
#include "stc/arc.h"
