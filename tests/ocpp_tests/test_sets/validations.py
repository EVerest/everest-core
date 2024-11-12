# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import json
import logging
import time
import asyncio
from datetime import datetime, timedelta
from dateutil import parser
import OpenSSL.crypto as crypto
from ocpp.messages import unpack

from ocpp.v16 import call_result
from ocpp.v201.datatypes import MeterValueType, SampledValueType

from everest.testing.ocpp_utils.charge_point_utils import ValidationMode

VALID_ID_TAG_1 = "RFID_VALID1"
VALID_ID_TAG_2 = "RFID_VALID2"
INVALID_ID_TAG = "RFID_INVALID"
PARENT_ID_TAG = "PARENT"
STANDARD_TRANSACTION_ID = 1


def validate_standard_start_transaction(meta_data, msg, exp_payload):

    if msg.action != "StartTransaction":
        return False

    success = (
        msg.payload["connectorId"] == exp_payload.connector_id
        and (msg.payload["idTag"] == exp_payload.id_tag or exp_payload.id_tag == None)
        and msg.payload["meterStart"] == exp_payload.meter_start
        and "timestamp" in msg.payload
    )

    if success:
        return True
    elif not success and meta_data.validation_mode == ValidationMode.STRICT:
        assert False
    else:
        return False


def validate_standard_stop_transaction(meta_data, msg, exp_payload):

    if msg.action != "StopTransaction":
        return False

    success = (
        "meterStop" in msg.payload
        and msg.payload["reason"] == exp_payload.reason
        and (
            msg.payload["transactionId"] == exp_payload.transaction_id
            or msg.payload["transactionId"] == STANDARD_TRANSACTION_ID
        )
    )

    if exp_payload.id_tag != None:
        success = success and msg.payload["idTag"] == exp_payload.id_tag

    if exp_payload.transaction_data != None:
        success = success and "transactionData" in msg.payload

    if success:
        return True
    elif not success and meta_data.validation_mode == ValidationMode.STRICT:
        assert False
    else:
        return False


def validate_remote_start_stop_transaction(meta_data, msg, exp_payload):
    success = msg.payload["status"] == exp_payload.status
    if success:
        return True
    elif not success and meta_data.validation_mode == ValidationMode.STRICT:
        assert False
    else:
        return False


def validate_meter_values(
    messages,
    periodic_measurands,
    clock_aligned_measurands,
    periodic_interval,
    clock_aligned_interval,
):

    periodic_meter_values = []
    clock_aligned_meter_values = []
    for msg in messages:
        if (
            msg.payload["meterValue"][0]["sampledValue"][0]["context"]
            == "Sample.Periodic"
        ):
            periodic_meter_values.extend(msg.payload["meterValue"])
        elif (
            msg.payload["meterValue"][0]["sampledValue"][0]["context"] == "Sample.Clock"
        ):
            clock_aligned_meter_values.extend(msg.payload["meterValue"])

    validate_interval(periodic_meter_values, periodic_interval)
    validate_interval(clock_aligned_meter_values, clock_aligned_interval)

    validate_clock_alignment(clock_aligned_meter_values, clock_aligned_interval)

    validate_measurands(periodic_meter_values, periodic_measurands)
    validate_measurands(clock_aligned_meter_values, clock_aligned_measurands)

    return True


def validate_clock_alignment(meter_values, interval):

    if interval == 0:
        return True

    for meter_value in meter_values:
        dt = parser.parse(meter_value["timestamp"])
        diff = (datetime.min - dt.replace(tzinfo=None)) % timedelta(seconds=interval)
        if diff.seconds > 2 and diff.minutes == 0 and diff.hours == 0:
            return False
    return True


def validate_interval(meter_values, interval):
    if len(meter_values) <= 1:
        return True

    i = 0
    while i < len(meter_values) - 1:
        x = meter_values[i]
        y = meter_values[i + 1]
        x_ts = parser.parse(x["timestamp"]).timestamp()
        y_ts = parser.parse(y["timestamp"]).timestamp()
        diff = y_ts - x_ts
        if abs(diff - interval) > 1:
            return False
        i += 1

    return True


def validate_measurands(meter_values, measurands):
    for measurand in measurands:
        found = False
        for meter_value in meter_values:
            for sampled_meter_value in meter_value["sampledValue"]:
                if measurand == sampled_meter_value["measurand"]:
                    found = True
            if not found:
                return False
    return True


def dont_validate_meter_values(x, y, z):
    return True


def dont_validate_sign_certificate(x, y, z):
    return True


def dont_validate_boot_notification(x, y, z):
    return True


def validate_composite_schedule(
    meta_data, msg, exp_payload: call_result.GetCompositeSchedulePayload
):
    return (
        msg.payload["status"] == exp_payload.status
        and msg.payload["connectorId"] == exp_payload.connector_id
        and msg.payload["chargingSchedule"]["chargingRateUnit"]
        == exp_payload.charging_schedule.charging_rate_unit
        and validate_duration(
            msg.payload["chargingSchedule"]["duration"],
            exp_payload.charging_schedule.duration,
        )
        and validate_charging_schedule_periods(
            msg.payload["chargingSchedule"]["chargingSchedulePeriod"],
            exp_payload.charging_schedule.charging_schedule_period,
        )
    )


def validate_duration(duration, exp_duration):
    return (
        duration == exp_duration
        or duration - 2 == exp_duration
        or duration + 2 == exp_duration
    )


def validate_charging_schedule_periods(periods, exp_periods):
    success = len(periods) >= len(exp_periods)
    if success:
        i = 0
        for i in range(len(exp_periods)):
            if periods[i]["limit"] != exp_periods[i].limit:
                return False
            elif (
                periods[i]["startPeriod"] != exp_periods[i].start_period
                and periods[i]["startPeriod"] != exp_periods[i].start_period + 1
                and periods[i]["startPeriod"] != exp_periods[i].start_period - 1
            ):
                return False
            elif (
                exp_periods[i].number_phases != None
                and periods[i]["numberPhases"] != exp_periods[i].number_phases
            ):
                return False
            i += 1
        return True
    else:
        return False


def validate_security_event_notification(meta_data, msg, exp_payload):
    return msg.payload["type"] == exp_payload.type


def validate_get_log(meta_data, msg, exp_payload):
    return msg.payload["status"] == exp_payload.status


def validate_boot_notification(meta_data, msg, exp_payload):
    return (
        msg.payload["chargeBoxSerialNumber"] == exp_payload.charge_box_serial_number
        and msg.payload["chargePointModel"] == exp_payload.charge_point_model
        and msg.payload["chargePointVendor"] == exp_payload.charge_point_vendor
    )


def validate_status_notification_201(meta_data, msg, exp_payload):
    return (
        msg.payload["connectorStatus"] == exp_payload.connector_status
        and msg.payload["evseId"] == exp_payload.evse_id
        and msg.payload["connectorId"] == exp_payload.connector_id
    )


def validate_notify_report_data_201(meta_data, msg, exp_payload):
    found_items = 0

    for payload in exp_payload.report_data:
        el = find_report_data(payload, msg.payload["reportData"])
        if el != None:
            if (
                msg.payload["requestId"] == exp_payload.request_id
                and payload.variable_attribute.type
                == el["variableAttribute"][0]["type"]
                and payload.variable_attribute.value
                == el["variableAttribute"][0]["value"]
            ):
                found_items += 1
    if found_items == len(exp_payload.report_data):
        return True
    else:
        return False


def find_report_data(report_data_element, report_data_list):
    for el in report_data_list:
        if (
            el["component"]["name"] == report_data_element.component.name
            and el["variable"]["name"] == report_data_element.variable.name
        ):
            # check if evse id has to be checked
            if report_data_element.component.evse != None:
                if (
                    report_data_element.component.evse.id
                    == el["component"]["evse"]["id"]
                ):
                    return el
            # check if variable instance has to be checked
            elif report_data_element.variable.instance != None:
                if report_data_element.variable.instance == el["variable"]["instance"]:
                    return el
            else:
                return el
    return None


def validate_data_transfer_pnc_get_15118_ev_certificate(meta_data, msg, exp_payload):
    return (
        msg.payload["vendorId"] == exp_payload.vendor_id
        and msg.payload["messageId"] == exp_payload.message_id
        and "action" in msg.payload["data"]
        and "exiRequest" in msg.payload["data"]
        and "iso15118SchemaVersion" in msg.payload["data"]
    )


def validate_data_transfer_sign_certificate(meta_data, msg, exp_payload):
    data = json.loads(msg.payload["data"])
    try:
        return (
            msg.payload["vendorId"] == exp_payload.vendor_id
            and msg.payload["messageId"] == exp_payload.message_id
            and "certificateType" in data
            and "csr" in data
            and data["certificateType"] == "V2GCertificate"
            and crypto.load_certificate_request(crypto.FILETYPE_PEM, data["csr"])
        )
    except Exception:
        return False


async def wait_for_callerror_and_validate(
    meta_data, charge_point, exp_payload, validate_payload_func=None, timeout=30
):
    """
    This method waits for a CallError message
    """

    logging.debug(f"Waiting for CallError")

    # check if expected message has been sent already
    if (
        meta_data.validation_mode == ValidationMode.EASY
        and validate_call_error_against_old_messages(meta_data, exp_payload)
    ):
        logging.debug(
            f"Found correct CallError message with payload {exp_payload} in old messages"
        )
        logging.debug("OK!")
        return True

    t_timeout = time.time() + timeout
    while time.time() < t_timeout:
        try:
            raw_message = await asyncio.wait_for(
                charge_point.wait_for_message(), timeout=timeout
            )
            charge_point.message_event.clear()
            msg = unpack(raw_message)
            if msg.message_type_id == 4:
                return validate_call_error(msg, exp_payload)
        except asyncio.TimeoutError as ex:
            logging.debug("Timeout while waiting for new message")

    logging.info(f"Timeout while waiting for CallError message")
    logging.info("This is the message history")
    charge_point.message_history.log_history()
    return False


def validate_call_error(msg, exp_payload):
    if msg.message_type_id == 4:
        logging.debug("Received CallError")
        if msg.error_code == exp_payload:
            return True
        else:
            logging.error(
                f'Wrong error code "{msg.error_code}" expected "{exp_payload}"'
            )
            return False
    return False


def validate_call_error_against_old_messages(meta_data, exp_payload):
    if meta_data.messages:
        for msg in meta_data.messages:
            success = validate_call_error(msg, exp_payload)
            if success:
                meta_data.messages.remove(msg)
                return True
    return False


def validate_transaction_event_started(meta_data, msg, exp_payload):
    return msg.payload["eventType"] == exp_payload.event_type


def validate_measurands_match(meter_value: MeterValueType, expected_measurands):
    reported_measurands = []
    for element in meter_value.sampled_value:
        sampled_value: SampledValueType = SampledValueType(**element)
        if sampled_value.measurand not in reported_measurands:
            reported_measurands.append(sampled_value.measurand)

    return expected_measurands == reported_measurands
