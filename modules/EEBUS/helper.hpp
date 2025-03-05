#ifndef MODULES_EEBUS_HELPER_HPP
#define MODULES_EEBUS_HELPER_HPP

#include "control_service/control_service.grpc.pb.h"
#include "usecases/cs/lpc/service.grpc.pb.h"

#include <grpcpp/grpcpp.h>

namespace module {

bool compare_use_case(
    const control_service::UseCase& uc1,
    const control_service::UseCase& uc2
);

} // namespace module

#endif
