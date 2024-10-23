#include "fuzz_cmn.h"

void nfcsnoop_capture(NFC_HDR const*, bool){};
void delete_stack_non_volatile_store(bool){};
void debug_nfcsnoop_dump(int){};
std::string nfc_storage_path;

uint8_t appl_dta_mode_flag = 0;
bool nfc_debug_enabled = true;

#ifdef STANDALONE_FUZZER
int main(int argc, char* argv[]) {
  uint_t iterations = 50;
  uint_t seed = 0;

  if (argc >= 2) {
    seed = atol(argv[1]);
  }

  if (argc >= 3) {
    iterations = atol(argv[2]);
  }

  FUZZLOG("iterations=%d, seed=%d", iterations, seed);

  if (0 != LLVMFuzzerInitialize(&argc, &argv)) {
    return -1;
  }

  for (auto i = 0; i < iterations; i++) {
    FUZZLOG("iteration=%d, seed=%d", i, seed);
    srandom(seed);
    seed = random();
    auto data = FuzzSeqGen(3, 255);
    if (!LLVMFuzzerTestOneInput(&data[0], data.size())) {
      break;
    }
  }

  return 0;
}
#endif
