#ifndef __STATS_LOG_H__
#define __STATS_LOG_H__

#include <log/log.h>
#include <sys/time.h>

// Dummy definitions to get rid of unnecessary dependency

namespace android {
namespace util {
enum {
  NFC_HCE_TRANSACTION_OCCURRED,
  NFC_ERROR_OCCURRED,
};

void stats_write(int32_t, ...);
}  // namespace android
}
#endif
