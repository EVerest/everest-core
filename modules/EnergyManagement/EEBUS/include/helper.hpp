// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MODULES_EEBUS_HELPER_HPP
#define MODULES_EEBUS_HELPER_HPP

#include "common_types/types.grpc.pb.h"
#include <generated/types/energy.hpp>

namespace module {

types::energy::ExternalLimits translate_to_external_limits(const common_types::LoadLimit& load_limit);

} // namespace module

#endif
