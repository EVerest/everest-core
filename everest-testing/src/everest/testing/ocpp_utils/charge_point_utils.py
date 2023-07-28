# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

from datetime import datetime
from pathlib import Path
import OpenSSL.crypto as crypto
import logging
import time
import asyncio
from enum import Enum
from dataclasses import dataclass
from typing import Optional

from ocpp.messages import unpack
from ocpp.charge_point import ChargePoint as CP
from ocpp.charge_point import snake_to_camel_case, camel_to_snake_case, asdict, remove_nones


@dataclass
class ChargePointInfo:
    charge_point_id: str = "cp001"
    charge_point_vendor: Optional[str] = None
    charge_point_model: Optional[str] = None
    firmware_version: Optional[str] = None


@dataclass
class AuthorizationInfo:
    emaid: str
    valid_id_tag_1: str
    valid_id_tag_2: str
    invalid_id_tag: str
    parent_id_tag: str
    invalid_parent_id_tag: str


@dataclass
class CertificateInfo:
    csms_root_ca: Path
    csms_root_ca_key: Path
    csms_root_ca_invalid: Path
    csms_cert: Path
    csms_key: Path
    csms_passphrase: str
    mf_root_ca: Path


@dataclass
class FirmwareInfo:
    update_file: Path
    update_file_signature: Path


@dataclass
class OcppTestConfiguration:
    csms_port: str = 9000
    charge_point_info: ChargePointInfo = ChargePointInfo()
    config_path: Optional[Path] = None
    authorization_info: Optional[AuthorizationInfo] = None
    certificate_info: Optional[CertificateInfo] = None
    firmware_info: Optional[FirmwareInfo] = None


class ValidationMode(str, Enum):
    STRICT = "STRICT"
    EASY = "EASY"


class TestUtility:
    __test__ = False

    def __init__(self) -> None:
        self.messages = []
        self.validation_mode = ValidationMode.EASY
        self.forbidden_actions = []


async def wait_for_and_validate(meta_data: TestUtility, charge_point: CP, exp_action: str,
                                exp_payload, validate_payload_func=None, timeout: int = 30):

    """This method waits for an expected message specified by the message_type, the action and the payload to be received. 
    It also considers the meta_data that contains the message history, the validation mode and forbidden actions. 

    Args:
        meta_data (TestUtility): contains the message history, the validation mode and forbidden actions that are considered in the validation
        charge_point (CP): The instance of the wrapper around the chargepoint websocket connection
        exp_action (str): The expected OCPP action (e.g. StatusNotification, BootNotification, etc.)
        exp_payload (_type_): The expected payload. Can be of type dict or can also be a call or call_result of the ocpp lib. If a dict is given, 
        only the subset of the entries given in the dict will be validated 
        validate_payload_func (function, optional): Optional validation function that can be used for more complex validations. Defaults to None.
        timeout (int, optional): time in seconds until waiting for the exp_payload times out. Defaults to 30.

    Returns:
        _type_: _description_
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

            response = validate_message(
                msg, exp_action, exp_payload, validate_payload_func, meta_data)
            if response != False:
                logging.debug("Message validated successfully!")
                meta_data.messages.remove(msg)
                if response:
                    return response
                else:
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
            response = validate_message(
                msg, exp_action, exp_payload, validate_payload_func, meta_data)
            if response:
                meta_data.messages.remove(msg)
                return response
    return False


def validate_message(msg, exp_action, exp_payload, validate_payload_func, meta_data):

    if msg.action in meta_data.forbidden_actions:
        logging.error(
            f"Forbidden action {msg.action} was sent by the charge point")
        assert False

    try:
        if ((msg.message_type_id == 2 and msg.action == exp_action) or msg.message_type_id == 3):
            if (validate_payload_func == None):
                if not isinstance(exp_payload, dict):
                    exp_payload = asdict(exp_payload)
                exp_payload = remove_nones(snake_to_camel_case(exp_payload))
                success = True
                for k, v in exp_payload.items():
                    if k not in msg.payload or msg.payload[k] != v:
                        success = False
                if success:
                    return camel_to_snake_case(msg.payload)
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
