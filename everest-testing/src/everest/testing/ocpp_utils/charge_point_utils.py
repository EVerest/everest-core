# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

from datetime import datetime
import OpenSSL.crypto as crypto
import logging
import time
import asyncio
from enum import Enum

from ocpp.messages import unpack
from ocpp.charge_point import snake_to_camel_case, asdict, remove_nones


class ValidationMode(str, Enum):
    STRICT = "STRICT"
    EASY = "EASY"


class TestUtility:
    def __init__(self) -> None:
        self.messages = []
        self.validation_mode = ValidationMode.EASY
        self.forbidden_actions = []


async def wait_for_and_validate(meta_data, charge_point, exp_action,
                                exp_payload, validate_payload_func=None, timeout=30):
    """
    This method waits for an expected message specified by the
    message_type, the action and the payload to be received
    """

    logging.debug(f"Waiting for {exp_action}")

    # check if expected message has been sent already
    if (meta_data.validation_mode == ValidationMode.EASY and
        validate_against_old_messages(meta_data,
                                      exp_action, exp_payload, validate_payload_func)):
        logging.debug(
            f"Found correct message {exp_action} with payload {exp_payload} in old messages")
        logging.debug("OK!")
        return True

    t_timeout = time.time() + timeout
    while (time.time() < t_timeout):
        try:
            raw_message = await asyncio.wait_for(charge_point.wait_for_message(), timeout=timeout)
            charge_point.message_event.clear()
            msg = unpack(raw_message)
            if (msg.action != None):
                logging.debug(f"Received Call {msg.action}")
            elif (msg.message_type_id == 3):
                logging.debug("Received CallResult")

            meta_data.messages.append(msg)

            success = validate_message(
                msg, exp_action, exp_payload, validate_payload_func, meta_data)
            if success:
                logging.debug("OK!")
                meta_data.messages.remove(msg)
                return True
            else:
                logging.debug(
                    f"This message {msg.action} with payload {msg.payload} was not what I waited for")
                logging.debug(f"I wait for {exp_payload}")
                # add msg to messages and wait for next message
                meta_data.messages.append(msg)
        except asyncio.TimeoutError:
            logging.debug("Timeout while waiting for new message")

    logging.info(
        f"Timeout while waiting for correct message with action {exp_action} and payload {exp_payload}")
    logging.info("This is the message history")
    charge_point.message_history.log_history()
    return False


def validate_against_old_messages(meta_data, exp_action, exp_payload, validate_payload_func=None):
    if meta_data.messages:
        for msg in meta_data.messages:
            success = validate_message(
                msg, exp_action, exp_payload, validate_payload_func, meta_data)
            if success:
                meta_data.messages.remove(msg)
                return True
    return False


def validate_message(msg, exp_action, exp_payload, validate_payload_func, meta_data):

    if msg.action in meta_data.forbidden_actions:
        logging.error(
            f"Forbidden action {msg.action} was sent by the charge point")
        assert False

    try:
        if ((msg.message_type_id == 2 and msg.action == exp_action) or msg.message_type_id == 3):
            if (validate_payload_func == None):
                success = msg.payload == remove_nones(
                    snake_to_camel_case(asdict(exp_payload)))
                if success:
                    return True
                elif not success and meta_data.validation_mode == ValidationMode.STRICT and \
                        msg.message_type_id != 3:
                    assert False
                else:
                    return False
            else:
                return validate_payload_func(meta_data, msg, exp_payload)

        else:
            return False
    except KeyError:
        return False


class HistoryMessage:
    def __init__(self, message, initiator) -> None:
        self.message = message
        self.initiator = initiator
        self.time = datetime.now()


class MessageHistory:
    def __init__(self) -> None:
        self.messages = []

    def add_received(self, message):
        self.messages.append(HistoryMessage(message, "Chargepoint"))

    def add_send(self, message):
        self.messages.append(HistoryMessage(message, "CSMS"))

    def log_history(self):
        for message in self.messages:
            time = message.time.strftime("%d-%m-%Y, %H:%M:%S")
            logging.info(f"{time} {message.initiator}: {message.message}")


def create_cert(serial_no, not_before, not_after, ca_cert, csr, ca_private_key):
    cert = crypto.X509()
    cert.set_serial_number(serial_no)
    cert.gmtime_adj_notBefore(0)
    cert.gmtime_adj_notAfter(not_after)
    cert.set_issuer(ca_cert.get_subject())
    cert.set_subject(csr.get_subject())
    cert.set_pubkey(csr.get_pubkey())
    cert.sign(ca_private_key, 'SHA256')

    return crypto.dump_certificate(crypto.FILETYPE_PEM, cert)
