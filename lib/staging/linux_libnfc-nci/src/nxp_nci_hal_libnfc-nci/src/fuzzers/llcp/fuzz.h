#ifndef __FUZZ_H__
#define __FUZZ_H__

#include "fuzz_cmn.h"
#include "llcp_int.h"

enum FuzzType_t { Fuzz_Dummy, Fuzz_TypeMax };

extern tNFC_CONN_CBACK* rf_cback;

extern void ce_init();

#endif
