# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import asyncio
import logging

logger = logging.getLogger(__name__)


async def async_get(q: asyncio.Queue, timeout: float = None):
    """
    Asynchronously gets an item from an asyncio.Queue with a timeout.
    """
    try:
        return await asyncio.wait_for(q.get(), timeout=timeout)
    except asyncio.TimeoutError:
        raise TimeoutError(f"async_get timed out after {timeout} seconds")


async def async_wait_for(q: asyncio.Queue, expected, timeout: float):
    """Wait for a specific value to appear on a queue, draining intermediate events.

    Keeps pulling items from *q* until one compares equal to *expected* or
    *timeout* seconds elapse.  Intermediate items (e.g. state-transition
    limits that arrive before the one the test cares about) are logged and
    discarded.

    Returns the matching item on success; raises ``TimeoutError`` if the
    deadline passes without a match.
    """
    loop = asyncio.get_event_loop()
    deadline = loop.time() + timeout
    discarded = []
    while True:
        remaining = deadline - loop.time()
        if remaining <= 0:
            raise TimeoutError(
                f"async_wait_for timed out after {timeout}s. "
                f"Expected:\n  {expected}\nDiscarded {len(discarded)} items:\n"
                + "\n".join(f"  {d}" for d in discarded)
            )
        try:
            item = await asyncio.wait_for(q.get(), timeout=remaining)
        except asyncio.TimeoutError:
            raise TimeoutError(
                f"async_wait_for timed out after {timeout}s. "
                f"Expected:\n  {expected}\nDiscarded {len(discarded)} items:\n"
                + "\n".join(f"  {d}" for d in discarded)
            )
        if item == expected:
            return item
        logger.debug("async_wait_for: discarding intermediate item: %s", item)
        discarded.append(item)
