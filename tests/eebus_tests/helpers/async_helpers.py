# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import asyncio

async def async_get(q: asyncio.Queue, timeout: float = None):
    """
    Asynchronously gets an item from an asyncio.Queue with a timeout.
    """
    try:
        return await asyncio.wait_for(q.get(), timeout=timeout)
    except asyncio.TimeoutError:
        raise TimeoutError(f"async_get timed out after {timeout} seconds")
