#include "sip_core/_internal/sip_core.h"
#include "sip_core/sip_core.h"

static inline cme_error_t
__SipCore_create(event_loop_t evl, ip_t ip_addr,
                 enum SupportedSipTranspProtos proto_type, sip_core_t *out) {
  struct __SipCorePtr *sip_corep = malloc(sizeof(struct __SipCorePtr));
  struct __SipCore sip_core = {0};
  cme_error_t err;

  if (!sip_corep) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `struct __SipCorePtr`");
    goto error_out;
  }

  err = sip_transp_create(evl, ip_addr, proto_type, &sip_core.sip_transp);
  if (err) {
    goto error_corep_cleanup;
  }

  err = sip_transp_listen(sip_core.sip_transp, __SipCore_sip_transp_recvh,
                          sip_corep);
  if (err) {
    goto error_corep_cleanup;
  }

  sip_core.evl = event_loop_ref(evl);
  sip_core.listeners = queue__SipCoreListenersQueue_init();
  sip_core.stranses = hmap__SipCoreStransMap_init();

  *sip_corep = __SipCorePtr_from(sip_core);

  *out = sip_corep;

  return 0;

error_corep_cleanup:
  __SipCorePtr_drop(sip_corep);
error_out:
  *out = NULL;
  return cme_return(err);
};

static inline sip_core_t __SipCore_ref(sip_core_t sip_corep) {
  __SipCorePtr_clone(*sip_corep);

  return sip_corep;
}

static inline void __SipCore_deref(sip_core_t sip_corep) {
  int32_t usage_count = __SipCorePtr_use_count(sip_corep);

  __SipCorePtr_drop(sip_corep);

  // If usage count is 1 before drop it means it will be 0
  //  after drop but ptr holding usage count get freed on drop.
  if (usage_count <= 1) {
    free(sip_corep);
  }
}

cme_error_t sip_core_create(event_loop_t evl, ip_t ip_addr,
                            enum SupportedSipTranspProtos proto_type,
                            sip_core_t *out) {
  return __SipCore_create(evl, ip_addr, proto_type, out);
};

sip_core_t sip_core_ref(sip_core_t sip_corep) {
  return __SipCore_ref(sip_corep);
};

void sip_core_deref(sip_core_t sip_corep) { __SipCore_deref(sip_corep); };

cme_error_t sip_core_listen(sip_core_request_handler_t requesth, void *data,
                            sip_core_t sip_core) {
  return __SipCore_listen(requesth, data, sip_core);
}

static inline void __SipCore_destroy(void *data) {
  struct __SipCore *sip_core = data;

  queue__SipCoreListenersQueue_drop(&sip_core->listeners);
  hmap__SipCoreStransMap_drop(&sip_core->stranses);
  sip_transp_deref(sip_core->sip_transp);
  event_loop_deref(sip_core->evl);
};

static inline struct __SipCore __SipCore_clone(struct __SipCore sip_core) {
  return sip_core;
};
