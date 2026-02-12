#ifndef MODULES_EEBUS_HELPER_HPP
#define MODULES_EEBUS_HELPER_HPP

// everest core deps
#include <grpcpp/grpcpp.h>

// generated
#include "control_service/control_service.grpc.pb.h"
#include "usecases/cs/lpc/service.grpc.pb.h"

// module internal
#include "EEBUS.hpp"

namespace module {

types::energy::ExternalLimits translate_to_external_limits(const common_types::LoadLimit& load_limit);

} // namespace module

#endif
