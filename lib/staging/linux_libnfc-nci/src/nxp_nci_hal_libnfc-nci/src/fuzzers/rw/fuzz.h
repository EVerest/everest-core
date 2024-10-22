#ifndef __FUZZ_H__
#define __FUZZ_H__

#include "fuzz_cmn.h"
#include "rw_int.h"

enum FuzzType_t {
  Fuzz_Type1,
  Fuzz_Type2,
  Fuzz_Type3,
  Fuzz_Type4,
  Fuzz_Type5,
  Fuzz_Mfc,

  Fuzz_TypeMax
};

extern "C" size_t LLVMFuzzerMutate(uint8_t* Data, size_t Size, size_t MaxSize);

extern tNFC_CONN_CBACK* rf_cback;

extern void rw_init();

#if 0
class Fuzz_Context
{
public:
  uint8_t                     SubType;
  const std::vector<bytes_t>  Data;
  std::unique_ptr<uint8_t[]>  ScratchPtr;

public:
  Fuzz_Context(uint8_t FuzzSubType, const std::vector<bytes_t>& Packets)
    : SubType(FuzzSubType)
    , Data(Packets)
  {
  }

  ~Fuzz_Context(){}
};
#endif

#endif
