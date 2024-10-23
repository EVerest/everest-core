#ifndef __FUZZ_H__
#define __FUZZ_H__

#include "ce_int.h"
#include "fuzz_cmn.h"

enum FuzzType_t {
  Fuzz_Type3,
  Fuzz_Type4,

  Fuzz_TypeMax
};

extern tNFC_CONN_CBACK* rf_cback;

extern void ce_init();

#endif
