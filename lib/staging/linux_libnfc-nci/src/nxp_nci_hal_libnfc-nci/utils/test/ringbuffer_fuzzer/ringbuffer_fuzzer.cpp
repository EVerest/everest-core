#include <sys/types.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "ringbuffer.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
  if (Size < 2) {
    return 0;
  }

  // Only allocate up to 1 << 16 bytes of memory. We shouldn't ever be
  // exercising more than this.
  uint16_t buffer_size = *((const uint16_t*)Data);
  ringbuffer_t* buffer = ringbuffer_init(buffer_size);

  if (buffer == nullptr) {
    return 0;
  }

  for (size_t i = 2; i < Size;) {
    size_t bytes_left = Size - i - 1;
    switch (Data[i++] % 6) {
      case 0: {
        ringbuffer_available(buffer);
        break;
      }
      case 1: {
        ringbuffer_size(buffer);
        break;
      }
      case 2: {
        if (bytes_left < 2) {
          break;
        }

        size_t bytes_to_insert = std::min(bytes_left - 1, (size_t)Data[i++]);
        ringbuffer_insert(buffer, &Data[i], bytes_to_insert);
        i += bytes_to_insert;
        break;
      }
      case 3: {
        if (bytes_left < 2) {
          break;
        }

        size_t bytes_to_grab = Data[i++];
        uint8_t* copy_buffer = (uint8_t*)malloc(bytes_to_grab);
        off_t offset = 0;
        if (ringbuffer_size(buffer) != 0) {
          offset = Data[i++] % ringbuffer_size(buffer);
        }

        ringbuffer_peek(buffer, offset, copy_buffer, (size_t)bytes_to_grab);
        free(copy_buffer);
        break;
      }
      case 4: {
        if (bytes_left < 1) {
          break;
        }

        size_t bytes_to_grab = Data[i++];
        uint8_t* copy_buffer = (uint8_t*)malloc(bytes_to_grab);
        ringbuffer_pop(buffer, copy_buffer, bytes_to_grab);
        free(copy_buffer);
        break;
      }
      case 5: {
        if (bytes_left < 1) {
          break;
        }
        ringbuffer_delete(buffer, (size_t)Data[i++]);
      }
    }
  }

  ringbuffer_free(buffer);
  return 0;
}
