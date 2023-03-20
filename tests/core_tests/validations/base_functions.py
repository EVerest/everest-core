# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import logging
import time
import asyncio
from typing import Callable
from typing import Any

from everest.testing.core_utils.test_control_module import TestControlModule

## helper functions

## ---------------------------------------------------
def get_key_if_exists(dataset:dict, keyset:list):
    """This function traverses a dataset (json tree-like data structure) for one or more (consecutive) key(s).
    It tries to match the keys form the 'keyset' list beginning from the dataset's root until it either locates
    the last entry in the keyset (which it then returns) or until a key is not found in the dataset (thus returning 'None').

    Args:
        dataset (dict): Tree-like data structure to search in.
        keyset (list): One or more consecutive keys to be found in the dataset.

    Returns:
        tuple(bool, Any): Key found (True/False), The contents of the found key or 'None'.
    """
    if not isinstance(dataset, dict):
        return False, None
    else:
        if keyset is not []:
            if len(keyset) == 1:
                if keyset[0] in dataset:
                    return True, dataset[keyset[0]]
                else:
                    return False, None
            else:
                return get_key_if_exists(dataset.get(keyset[0]), keyset[1:])


## base validation functions (call user validation functions according to generic schema)

## ---------------------------------------------------
async def wait_for_and_validate_event(test_control_module: TestControlModule,
                                      exp_event: str,
                                      exp_data: Any,
                                      validation_function: Callable = None,
                                      timeout=30):
    """This function waits for an expected event specified by exp_event and validates it

    Args:
        test_control_module (TestControlModule): The module keeping the event list
        exp_event (str): Expected event
        exp_data (Any): Expected data
        validation_function (Callable, optional): Function that can be used to validate data. Defaults to None.
                                                    If set to 'None', the appearance of the event itself marks success.
        timeout (int, optional): Specifies timeout after which validation will end. Defaults to 30.

    Returns:
        bool: True if validation was successful, else False
    """

    logging.debug(f"Waiting for event: \"{exp_event}\"")

    message = None
    t_timeout = time.time() + timeout
    while (time.time() < t_timeout):
        try:
            # iterate through event list and check if event has occurred
            event_list = test_control_module.get_event_list()

            for event in event_list[:]:
                if check_for_event(event, exp_event):
                    if validation_function != None:
                        success, message = validation_function(event, exp_event, exp_data)
                    else:
                        success = True # if "validation_function" is not set, the appearance of the event itself marks success
                    if success:
                        logging.info("Event criteria match!")
                        logging.debug(f"Event history: {test_control_module.get_event_list()}")
                        return True
                    else:
                        logging.debug(f"Event found [{event}], but criteria do not match!\n"
                                      f"Expected: {exp_data}\n"
                                      f"Event list: {event_list}\n"
                                      f"Message: {message}")

        except asyncio.TimeoutError as ex:
            logging.debug(f"Timeout while waiting for event!{ex}\n"
                          f"Message: {message}")

    logging.info(f"Timeout while waiting for event {exp_event} and data {exp_data}...\n"
                 f"Event history: {test_control_module.get_event_list()}\n"
                 f"Message: {message}")
    return False


## ---------------------------------------------------
def check_for_event(event, exp_event):
    """Simple helper function to check if the expected event name is in the received event list.

    Args:
        event (Any): Received event
        exp_event (Any): Expected event

    Returns:
        bool: True if expected event name is in received event, else False
    """
    event_found, _ = get_key_if_exists(event, [exp_event])
    if event_found:
        return True
    return False
