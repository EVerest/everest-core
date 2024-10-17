// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#ifndef V2G_CTX_H
#define V2G_CTX_H

#include "v2g.hpp"

#include <stdbool.h>

#define PHY_VALUE_MULT_MIN  -3
#define PHY_VALUE_MULT_MAX  3
#define PHY_VALUE_VALUE_MIN SHRT_MIN
#define PHY_VALUE_VALUE_MAX SHRT_MAX

struct v2g_context* v2g_ctx_create(evse_securityIntf* r_security);

/*!
 * \brief v2g_ctx_free
 * \param ctx
 */
void v2g_ctx_free(struct v2g_context* ctx);

#endif /* V2G_CTX_H */
