# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
from datetime import datetime, timedelta
import logging
import asyncio
import getpass

from ocpp.v16 import call, call_result
from ocpp.v16.enums import *

# fmt: off
from validations import (validate_get_log)
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest.testing.ocpp_utils.fixtures import *
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest_test_utils import *
# fmt: on


@pytest.mark.asyncio
async def test_get_diagnostics_retries(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_get_diagnostics_retries #########")

    await asyncio.sleep(1)

    # FIXME: make sure this port does not exist? or username and password are wrong?
    location = f"ftp://{getpass.getuser()}:12345@localhost:2121"
    start_time = datetime.utcnow()
    stop_time = start_time + timedelta(days=3)
    retries = 2
    retry_interval = 2

    test_utility.messages.clear()
    await charge_point_v16.get_diagnostics_req(
        location=location,
        start_time=start_time.isoformat(),
        stop_time=stop_time.isoformat(),
        retries=retries,
        retry_interval=retry_interval,
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.uploading),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.upload_failed),
    )

    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.uploading),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.upload_failed),
    )

    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.uploading),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.upload_failed),
    )


@pytest.mark.asyncio
async def test_upload_security_log_retries(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_upload_security_log_retries #########")

    oldest_timestamp = datetime.utcnow()
    latest_timestamp = oldest_timestamp + timedelta(days=3)
    retries = 2
    retry_interval = 2

    log = {
        "remoteLocation": f"ftp://{getpass.getuser()}:12345@localhost:2121",
        "oldestTimestamp": oldest_timestamp.isoformat(),
        "latestTimestamp": latest_timestamp.isoformat(),
    }

    test_utility.messages.clear()
    await charge_point_v16.get_log_req(
        log=log,
        log_type=Log.security_log,
        retries=retries,
        retry_interval=retry_interval,
        request_id=1,
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetLog",
        call_result.GetLogPayload(LogStatus.accepted),
        validate_get_log,
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "LogStatusNotification",
        call.LogStatusNotificationPayload(UploadLogStatus.uploading, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "LogStatusNotification",
        call.LogStatusNotificationPayload(UploadLogStatus.upload_failure, 1),
    )

    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "LogStatusNotification",
        call.LogStatusNotificationPayload(UploadLogStatus.uploading, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "LogStatusNotification",
        call.LogStatusNotificationPayload(UploadLogStatus.upload_failure, 1),
    )

    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "LogStatusNotification",
        call.LogStatusNotificationPayload(UploadLogStatus.uploading, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "LogStatusNotification",
        call.LogStatusNotificationPayload(UploadLogStatus.upload_failure, 1),
    )


@pytest.mark.asyncio
async def test_firwmare_update_retries(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    # not supported when implemented security extensions
    logging.info("######### test_firwmare_update_retries #########")

    await asyncio.sleep(1)

    retrieve_date = datetime.utcnow()
    location = f"ftp://{getpass.getuser()}:12345@localhost:2121/firmware_update.pnx"
    retries = 2
    retry_interval = 2

    await charge_point_v16.update_firmware_req(
        location=location,
        retrieve_date=retrieve_date.isoformat(),
        retries=retries,
        retry_interval=retry_interval,
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.downloading),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.download_failed),
    )

    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.downloading),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.download_failed),
    )

    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.downloading),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.download_failed),
    )


@pytest.mark.asyncio
async def test_signed_update_firmware_retries(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_signed_update_firmware_retries #########")

    await asyncio.sleep(1)

    await charge_point_v16.change_configuration_req(key="HeartbeatInterval", value="20")

    certificate = open(test_config.certificate_info.mf_root_ca).read()

    await charge_point_v16.install_certificate_req(
        certificate_type=CertificateUse.manufacturer_root_certificate,
        certificate=certificate,
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "InstallCertificate",
        call_result.InstallCertificatePayload(CertificateStatus.accepted),
    )

    location = f"ftp://{getpass.getuser()}:12345@localhost:2121/firmware_update.pnx"
    retries = 2
    retry_interval = 2
    retrieve_date_time = datetime.utcnow()
    mf_root_ca = open(test_config.certificate_info.mf_root_ca).read()
    fw_signature = open(test_config.firmware_info.update_file_signature).read()

    firmware = {
        "location": location,
        "retrieveDateTime": retrieve_date_time.isoformat(),
        "signingCertificate": mf_root_ca,
        "signature": fw_signature,
    }

    await charge_point_v16.signed_update_firmware_req(
        request_id=1, retries=retries, retry_interval=retry_interval, firmware=firmware
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedUpdateFirmware",
        call_result.SignedUpdateFirmwarePayload(UpdateFirmwareStatus.accepted),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.downloading, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.download_failed, 1),
    )

    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.downloading, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.download_failed, 1),
    )

    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.downloading, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.download_failed, 1),
    )

    # no SignedFirmwareStatusNotification.req should be sent anymore
    test_utility.forbidden_actions.append("SignedFirmwareStatusNotification")
    test_utility.messages.clear()
    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload()
    )
