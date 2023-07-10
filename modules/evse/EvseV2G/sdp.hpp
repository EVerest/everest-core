// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#ifndef SDP_H
#define SDP_H

#include "v2g.hpp"

int sdp_init(struct v2g_context* v2g_ctx);
int sdp_listen(struct v2g_context* v2g_ctx);

#endif /* SDP_H */
