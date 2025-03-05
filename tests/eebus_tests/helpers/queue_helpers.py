# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import queue
import time

def shutdown_queue_with_timeout(queue: queue.Queue, timeout: int):
    """
    Shutdown the queue with a timeout.

    It shuts down the queue and waits for it to be empty.
    If the queue is not empty after the timeout, it raises a TimeoutError.
    """    
    queue.shutdown()
    wait_for_queue_empty(queue, timeout)

def wait_for_queue_empty(queue: queue.Queue, timeout: int):
    """
    Wait for the queue to be empty.

    It waits for the queue to be empty for a given timeout.
    If the queue is not empty after the timeout, it raises a TimeoutError.
    """
    if timeout <= 0:
        raise ValueError("Timeout must be greater than 0")
    time_start = time.time()
    while not queue.empty():
        time_now = time.time()
        if time_now - time_start > timeout:
            raise TimeoutError("Queue is not empty after timeout")
        time.sleep(0.1)
