#include "fuzz_cmn.h"

std::string BytesToHex(const uint8_t* data, size_t size) {
  std::string result = "{";
  if (data && size) {
    StringAppendF(&result, "0x%02X", data[0]);
    for (auto i = 1; i < size; i++) {
      StringAppendF(&result, ", 0x%02X", data[i]);
    }
  }
  result += "}";

  return result;
}

std::string BytesToHex(const bytes_t& data) {
  return BytesToHex(&data[0], data.size());
}

static std::vector<bytes_t> UnpackPackets(const uint8_t* Data, size_t Size) {
  std::vector<bytes_t> result;
  while (Size > 0) {
    auto s = *Data++;
    Size--;

    if (s > Size) {
      s = Size;
    }

    if (s > 0) {
      result.push_back(bytes_t(Data, Data + s));
    }

    Size -= s;
    Data += s;
  }

  return result;
}

static size_t PackPackets(const std::vector<bytes_t>& Packets, uint8_t* Data,
                          size_t MaxSize) {
  size_t TotalSize = 0;

  for (auto it = Packets.cbegin(); MaxSize > 0 && it != Packets.cend(); ++it) {
    auto s = it->size();
    if (s == 0) {
      // skip empty packets
      continue;
    }

    if (s > MaxSize - 1) {
      s = MaxSize - 1;
    }
    *Data++ = (uint8_t)s;
    MaxSize--;

    memcpy(Data, it->data(), s);
    MaxSize -= s;
    Data += s;

    TotalSize += (s + 1);
  }

  return TotalSize;
}

extern "C" size_t LLVMFuzzerCustomMutator(uint8_t* Data, size_t Size,
                                          size_t MaxSize, uint Seed) {
  const uint MAX_PACKET_SIZE = 255;
  auto Packets = UnpackPackets(Data, Size);
  auto odd = Seed % 100;
  if (odd < 10 || Packets.size() == 0) {
    // ~10% chance to insert a new packet
    auto len = (Seed >> 8) % MAX_PACKET_SIZE;
    if (Packets.size() > 0) {
      auto pos = (Seed >> 16) % Packets.size();
      Packets.insert(Packets.begin() + pos, bytes_t(len));
    } else {
      Packets.push_back(bytes_t(len));
    }
  } else if (odd < 20 && Packets.size() > 1) {
    // ~10% chance to drop a packet
    auto pos = (Seed >> 16) % Packets.size();
    Packets.erase(Packets.begin() + pos);
  } else if (Packets.size() > 0) {
    // ~80% chance to mutate a packet, maximium length 255
    auto pos = (Seed >> 16) % Packets.size();
    auto& p = Packets[pos];

    auto size = p.size();
    p.resize(MAX_PACKET_SIZE);
    size = LLVMFuzzerMutate(p.data(), size, MAX_PACKET_SIZE);
    p.resize(size);
  }

  Fuzz_FixPackets(Packets, Seed);

  Size = PackPackets(Packets, Data, MaxSize);
  FUZZLOG("Packet size:%zu, data=%s", Packets.size(),
          BytesToHex(Data, Size).c_str());
  return Size;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
  const char* argv[] = {fuzzer_name};
  base::CommandLine::Init(1, argv);
  logging::SetLogItems(false, false, false, false);

  if (Size > 0) {
    auto Packets = UnpackPackets(Data, Size);
    Fuzz_RunPackets(Packets);
  }

  if (__gcov_flush) {
    __gcov_flush();
  }

  return 0;
}

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv) {
  base::CommandLine args(*argc, *argv);
  if (args.HasSwitch("v") || args.HasSwitch("verbose")) {
    nfc_debug_enabled = true;
    FUZZLOG("Debugging output enabled");
  } else {
    nfc_debug_enabled = false;
  }

  return 0;
}
