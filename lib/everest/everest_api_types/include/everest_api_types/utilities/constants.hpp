// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

namespace everest::lib::API {
#ifdef EVEREST_API_TYPES_LIB_MINIFIED_JSON
static const int json_indent = -1;
#else
static const int json_indent = 4;
#endif
} // namespace everest::lib::API
