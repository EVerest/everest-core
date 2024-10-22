#ifndef __FUZZ_CMN_H__
#define __FUZZ_CMN_H__

#include <base/command_line.h>
#include <base/logging.h>
#include <errno.h>
#include <semaphore.h>

#include <nfc_api.h>
#include <nfc_int.h>

#include <map>
#include <vector>

#include <android-base/stringprintf.h>
//using android::base::StringAppendF;
//using android::base::StringPrintf;

#define FUZZLOG(...)               \
  DLOG_IF(INFO, nfc_debug_enabled) \
      << __func__ << ":" << StringPrintf(__VA_ARGS__);

extern bool nfc_debug_enabled;

typedef std::vector<uint8_t> bytes_t;

std::string BytesToHex(const uint8_t* data, size_t size);
std::string BytesToHex(const bytes_t& data);
bytes_t FuzzSeqGen(size_t minLen, size_t maxLen);

extern void GKI_shutdown();

extern "C" int LLVMFuzzerInitialize(int*, char***);
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size);
extern "C" size_t LLVMFuzzerMutate(uint8_t* Data, size_t Size, size_t MaxSize);

extern "C" void __gcov_flush(void) __attribute__((weak));

class Fuzz_Context {
  std::vector<std::unique_ptr<uint8_t[]>> _ScratchPtrs;

 public:
  uint8_t SubType;
  const std::vector<bytes_t> Data;

 public:
  Fuzz_Context(uint8_t FuzzSubType, const std::vector<bytes_t>& Packets)
      : SubType(FuzzSubType), Data(Packets) {}

  uint8_t* GetBuffer(size_t size, const void* init_data = nullptr) {
    auto ptr = std::make_unique<uint8_t[]>(size);
    uint8_t* p = (uint8_t*)ptr.get();
    if (init_data) {
      memcpy(p, init_data, size);
    } else {
      memset(p, 0, size);
    }

    _ScratchPtrs.push_back(std::move(ptr));
    return p;
  }

  ~Fuzz_Context() {}
};

extern const char fuzzer_name[];
extern void Fuzz_FixPackets(std::vector<bytes_t>& Packets, uint Seed);
extern void Fuzz_RunPackets(const std::vector<bytes_t>& Packets);

#endif
