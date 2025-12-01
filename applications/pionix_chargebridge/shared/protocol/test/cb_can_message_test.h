#pragma once

#include "protocol/cb_can_message.h"
#include "protocol/cb_platform.h"
CB_STATIC_ASSERT(sizeof(CanErrorState) == 4, "CanErrorState data size!!");
CB_STATIC_ASSERT(sizeof(CanBitrate) == 4, "CanBitrate data size!!");
CB_STATIC_ASSERT(sizeof(CanFDBitrate) == 4, "CanFDBitrate data size!!");
CB_STATIC_ASSERT(sizeof(CanStatistics) == 4+4+4+4, "CanStatistics data size!!");
CB_STATIC_ASSERT(sizeof(struct cb_can_message) == 1+4+16+4+4+1+4+1+64+1, "cb_can_message type size!!");
