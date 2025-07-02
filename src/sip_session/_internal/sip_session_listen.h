#include "c_minilib_error.h"
#include "sip_session/sip_session.h"
#include "utils/memory.h"

struct __SipSessionListener {
  sip_session_next_stateh_t next_stateh;
  struct GenericPtr arg;
};

static inline void
__SipSessionListener_destroy(struct __SipSessionListener *sip_core){

};

static inline struct __SipSessionListener
__SipSessionListener_clone(struct __SipSessionListener sip_core) {
  return sip_core;
};

#define i_type __SipSessionListenerPtr
#define i_key struct __SipSessionListener
#define i_keydrop __SipSessionListener_destroy
#define i_keyclone __SipSessionListener_clone
#include "stc/arc.h"

static inline void
__SipSessionListener_create(struct __SipSessionListener listener,
                            struct __SipSessionListenerPtr *out) {
  *out = __SipSessionListenerPtr_from_ptr(my_malloc(sizeof(listener)));

  *out->get = listener;
}
