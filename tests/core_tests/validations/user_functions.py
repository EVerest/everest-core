# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

from validations.base_functions import get_key_if_exists

validation_memory = []


## (user-written) validation functions

## ---------------------------------------------------
def validate_transaction_charged_amount(event, exp_event, exp_data):
    """This function checks if an expected minimum amount of kWh has been charged.
    For this to work, a previous call of 'set_test_memory()' has to have been made with
    'transaction_started' as its target event.

    Args:
        event (Any): Received event
        exp_event (optional - not needed): (always assumed to be 'transaction_finished')
        exp_data (Any): Expected minimum amount of kWh to be charged.


    Returns:
        tuple(bool, str): True if valid result, else False. , result message (incl. error, if applicable)
    """
    global validation_memory

    _, pre_event_value_from_memory = get_key_if_exists(validation_memory[0], ['transaction_started', 'energy_Wh_import'])
    event_found, received_event_value = get_key_if_exists(event, ['transaction_finished', 'energy_Wh_import'])

    if event_found == False:
        return False, str("Error: Expected data is missing from event!")

    if (float(received_event_value) - float(pre_event_value_from_memory)) > float(exp_data):
        validation_memory = []
        return True, str("Ok")

    return False, str(f"Error: Event value ({received_event_value}) too low! "
                      f"[after subtracting pre-event value ({pre_event_value_from_memory}),"
                      f" the result is lower than the expected value ({exp_data})]")


## ---------------------------------------------------
def set_test_memory(event, exp_event, exp_data):
    """This function writes the content of the FIRST expected target event to validation memory (to be evaluated in another function later).
    Other occurrences receive a valid return value, but are not written to validation memory!

    Args:
        event (Any): Received event, including data to be set to validation memory.
        exp_event (Any): Expected event. (e.g. 'transaction_started')
        exp_data (optional - not needed): -

    Returns:
        tuple(bool, str): True if valid event, else False. , result message (incl. error, if applicable)
    """
    global validation_memory
    event_found, _ = get_key_if_exists(event, [exp_event])
    if event_found:
        if not validation_memory:
            validation_memory.append(event)
        return True, str("Ok")
    return False, str("Event not found!")


## ---------------------------------------------------
def append_test_memory(event, exp_event, exp_data):
    """This function adds (appends) the contents of ALL expected target events to validation memory
    (to be evaluated in another function later).

    Args:
        event (Any): Received event, including data to be set to validation memory.
        exp_event (Any): Expected event. (e.g. 'transaction_started')
        exp_data (optional - not needed): -

    Returns:
        tuple(bool, str): True if valid event, else False. , result message (incl. error, if applicable)
    """
    global validation_memory
    event_found, _ = get_key_if_exists(event, [exp_event])
    if event_found:
        validation_memory.append(event)
        return True, str("Ok")
    return False, str("Event not found!")