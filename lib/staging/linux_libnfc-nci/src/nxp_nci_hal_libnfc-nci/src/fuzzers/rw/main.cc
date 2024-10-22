#include "fuzz.h"

#define MODULE_NAME "nfc_rw_fuzzer"

const char fuzzer_name[] = MODULE_NAME;

extern void Type1_FixPackets(uint8_t SubType, std::vector<bytes_t>& Packets);
extern void Type2_FixPackets(uint8_t SubType, std::vector<bytes_t>& Packets);
extern void Type3_FixPackets(uint8_t SubType, std::vector<bytes_t>& Packets);
extern void Type4_FixPackets(uint8_t SubType, std::vector<bytes_t>& Packets);
extern void Type5_FixPackets(uint8_t SubType, std::vector<bytes_t>& Packets);
extern void Mfc_FixPackets(uint8_t SubType, std::vector<bytes_t>& Packets);

extern void Type1_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Packets);
extern void Type2_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Packets);
extern void Type3_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Packets);
extern void Type4_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Packets);
extern void Type5_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Packets);
extern void Mfc_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Packets);

void Fuzz_FixPackets(std::vector<bytes_t>& Packets, uint Seed) {
  if (Packets.size() < 2) {
    // At least two packets, first one is the control packet
    Packets.resize(2);
  }

  auto& ctrl = Packets[0];
  if (ctrl.size() != 2) {
    ctrl.resize(2);
    ctrl[0] = (Seed >> 16) & 0xFF;
    ctrl[1] = (Seed >> 24) & 0xFF;
  }

  uint8_t FuzzType = ctrl[0] % Fuzz_TypeMax;
  uint8_t FuzzSubType = ctrl[1];

  switch (FuzzType) {
    case Fuzz_Type1:
      Type1_FixPackets(FuzzSubType, Packets);
      break;

    case Fuzz_Type2:
      Type2_FixPackets(FuzzSubType, Packets);
      break;

    case Fuzz_Type3:
      Type3_FixPackets(FuzzSubType, Packets);
      break;

    case Fuzz_Type4:
      Type4_FixPackets(FuzzSubType, Packets);
      break;
    case Fuzz_Type5:
      Type5_FixPackets(FuzzSubType, Packets);
      break;
    case Fuzz_Mfc:
      Mfc_FixPackets(FuzzSubType, Packets);
      break;

    default:
      FUZZLOG("Unknown fuzz type %hhu", FuzzType);
      break;
  }
}

void Fuzz_RunPackets(const std::vector<bytes_t>& Packets) {
  if (Packets.size() < 2) {
    return;
  }

  auto& ctrl = Packets[0];
  if (ctrl.size() < 2) {
    return;
  }

  uint8_t FuzzType = ctrl[0] % Fuzz_TypeMax;
  uint8_t FuzzSubType = ctrl[1];

  FUZZLOG("Fuzzing Type%u tag", (uint)(FuzzType + 1));

  switch (FuzzType) {
    case Fuzz_Type1:
      Type1_Fuzz(FuzzSubType, Packets);
      break;

    case Fuzz_Type2:
      Type2_Fuzz(FuzzSubType, Packets);
      break;

    case Fuzz_Type3:
      Type3_Fuzz(FuzzSubType, Packets);
      break;

    case Fuzz_Type4:
      Type4_Fuzz(FuzzSubType, Packets);
      break;

    case Fuzz_Type5:
      Type5_Fuzz(FuzzSubType, Packets);
      break;

    case Fuzz_Mfc:
      Mfc_Fuzz(FuzzSubType, Packets);
      break;

    default:
      FUZZLOG("Unknown fuzz type: %hhu", FuzzType);
      break;
  }
}
