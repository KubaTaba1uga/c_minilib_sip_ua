/******************************************************************************
 *                             Sip Transport Ptr                              *
 ******************************************************************************/
typedef struct __SipTransport *__SipTransportPtr;

void __sip_transp_ptr_destroy(__SipTransportPtr *evlp);
__SipTransportPtr __sip_transp_ptr_clone(__SipTransportPtr evlp);

#define i_type sip_transp_ptr
#define i_key __SipTransportPtr
#define i_keydrop __sip_transp_ptr_destroy
#define i_keyclone __sip_transp_ptr_clone
#include "stc/arc.h"

typedef struct sip_transp_ptr sip_transp_ptr_t;
