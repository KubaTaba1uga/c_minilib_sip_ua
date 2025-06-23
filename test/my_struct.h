struct __MyStruct {
  uint32_t foo;
  int32_t bar;
};

static inline void __MyStruct_destroy(struct __MyStruct *ms) { return; };
static inline struct __MyStruct __MyStruct_clone(struct __MyStruct ms) {
  return ms;
};

#define i_type MyStructPtr
#define i_key struct __MyStruct
#define i_keydrop __MyStruct_destroy
#define i_keyclone __MyStruct_clone
#include "stc/arc.h"
