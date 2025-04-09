# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

# fmt: off
import os
import sys

from everest.testing.core_utils.controller.test_controller_interface import TestController

sys.path.append(os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../..")))
from everest.testing.ocpp_utils.fixtures import *
from ocpp.v201.enums import (CertificateSigningUseType)
from ocpp.v201 import call as call201
from ocpp.v16.enums import ChargePointErrorCode, ChargePointStatus, ConfigurationStatus
from ocpp.v16 import call
from ocpp.charge_point import asdict, remove_nones, snake_to_camel_case, camel_to_snake_case
from ocpp.routing import create_route_map
import asyncio
import pytest
from validations import (validate_standard_start_transaction,
                                    validate_data_transfer_pnc_get_15118_ev_certificate,
                                    validate_data_transfer_sign_certificate)
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP16ConfigAdjustment
from everest_test_utils import *
# fmt: on


def validate_authorize_req(
    authorize_req: call201.AuthorizePayload, contains_contract, contains_ocsp
):
    return (authorize_req.certificate != None) == contains_contract and (
        authorize_req.iso15118_certificate_hash_data != None
    ) == contains_ocsp


@pytest.mark.skip(
    "Plug and charge tests do currently interfere when they are run in parallel with other tests"
)
class TestPlugAndCharge:

    @pytest.mark.asyncio
    @pytest.mark.source_certs_dir(Path(__file__).parent.parent / "everest-aux/certs")
    async def test_contract_installation_and_authorization_01(
        self,
        request,
        central_system_v16: CentralSystem,
        charge_point_v16: ChargePoint16,
        test_controller: TestController,
        test_config,
        test_utility: TestUtility,
    ):
        """
        Test for contract installation on the vehicle and succeeding authorization and charging process
        """

        setattr(charge_point_v16, "on_data_transfer", on_data_transfer_accept_authorize)
        central_system_v16.chargepoint.route_map = create_route_map(
            central_system_v16.chargepoint
        )

        await asyncio.sleep(3)
        test_controller.plug_in_ac_iso()

        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "DataTransfer",
            call.DataTransferPayload(
                vendor_id="org.openchargealliance.iso15118pnc",
                message_id="Get15118EVCertificate",
                data=None,
            ),
            validate_data_transfer_pnc_get_15118_ev_certificate,
        )

        # expect authorize.req
        r: call.DataTransferPayload = call.DataTransferPayload(
            **await wait_for_and_validate(
                test_utility,
                charge_point_v16,
                "DataTransfer",
                {
                    "messageId": "Authorize",
                    "vendorId": "org.openchargealliance.iso15118pnc",
                },
            )
        )

        authorize_req: call201.AuthorizePayload = call201.AuthorizePayload(
            **camel_to_snake_case(json.loads(r.data))
        )
        assert validate_authorize_req(authorize_req, False, True)

        # expect StartTransaction.req
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StartTransaction",
            call.StartTransactionPayload(
                1, test_config.authorization_info.emaid, 0, ""
            ),
            validate_standard_start_transaction,
        )

        # expect StatusNotification with status charging
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.charging
            ),
        )

        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.available
            ),
        )

    @pytest.mark.asyncio
    async def test_contract_installation_and_authorization_02(
        self,
        request,
        central_system_v16: CentralSystem,
        charge_point_v16: ChargePoint16,
        test_controller: TestController,
        test_utility: TestUtility,
    ):
        """
        Test for contract installation on the vehicle and succeeding authorization request that is rejected by CSMS
        """

        setattr(charge_point_v16, "on_data_transfer", on_data_transfer_reject_authorize)
        central_system_v16.chargepoint.route_map = create_route_map(
            central_system_v16.chargepoint
        )

        await asyncio.sleep(3)
        test_controller.plug_in_ac_iso()

        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "DataTransfer",
            call.DataTransferPayload(
                vendor_id="org.openchargealliance.iso15118pnc",
                message_id="Get15118EVCertificate",
                data=None,
            ),
            validate_data_transfer_pnc_get_15118_ev_certificate,
        )

        # expect authorize.req
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "DataTransfer",
            {
                "messageId": "Authorize",
                "vendorId": "org.openchargealliance.iso15118pnc",
            },
        )

        test_utility.messages.clear()
        test_utility.forbidden_actions.append("StartTransaction")

        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.available
            ),
        )

    @pytest.mark.asyncio
    @pytest.mark.source_certs_dir(Path(__file__).parent.parent / "everest-aux/certs")
    async def test_contract_installation_and_authorization_03(
        self,
        request,
        central_system_v16: CentralSystem,
        charge_point_v16: ChargePoint16,
        test_controller: TestController,
        test_config,
        test_utility: TestUtility,
    ):
        """
        Test for contract installation on the vehicle and succeeding authorization and charging process
        """

        await charge_point_v16.change_configuration_req(
            key="CentralContractValidationAllowed", value="true"
        )

        certificate_hash_data = {
            "hashAlgorithm": "SHA256",
            "issuerKeyHash": "66fce9295edc049f4a183458948ecaa8e3558e4aa3041f13a2363d1d953d33e5",
            "issuerNameHash": "3a1ad85a129bd5db30c2f099a541f76e562b8a30e9f49f3f47077eeae3750a2a",
            "serialNumber": "3041",
        }

        delete_certificate_req = {"certificateHashData": certificate_hash_data}

        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="DeleteCertificate",
            data=json.dumps(delete_certificate_req),
        )

        # expect not found
        assert json.loads(data_transfer_response.data) == {"status": "Accepted"}

        setattr(charge_point_v16, "on_data_transfer", on_data_transfer_accept_authorize)
        central_system_v16.chargepoint.route_map = create_route_map(
            central_system_v16.chargepoint
        )

        test_controller.plug_in_ac_iso()
        # expect authorize.req
        r: call.DataTransferPayload = call.DataTransferPayload(
            **await wait_for_and_validate(
                test_utility,
                charge_point_v16,
                "DataTransfer",
                {
                    "messageId": "Authorize",
                    "vendorId": "org.openchargealliance.iso15118pnc",
                },
            )
        )

        authorize_req: call201.AuthorizePayload = call201.AuthorizePayload(
            **camel_to_snake_case(json.loads(r.data))
        )
        assert validate_authorize_req(authorize_req, True, False)

        # expect StartTransaction.req
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StartTransaction",
            call.StartTransactionPayload(
                1, test_config.authorization_info.emaid, 0, ""
            ),
            validate_standard_start_transaction,
        )

        # expect StatusNotification with status charging
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.charging
            ),
        )

        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.available
            ),
        )

    @pytest.mark.asyncio
    async def test_eim_01(
        self,
        test_config,
        charge_point_v16: ChargePoint16,
        test_controller: TestController,
        test_utility: TestUtility,
    ):
        """
        Test normal EIM authentication with swipe first.
        We should test that:
            - Charging process starts
            - DataTransfer(Authorize.req) is not transmitted in this case.
        """

        test_utility.forbidden_actions.append("DataTransfer")

        # swipe first
        test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

        # plug in second
        test_controller.plug_in_ac_iso()

        # expect StartTransaction.req
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StartTransaction",
            call.StartTransactionPayload(
                1, test_config.authorization_info.valid_id_tag_1, 0, ""
            ),
            validate_standard_start_transaction,
        )

        # expect StatusNotification with status charging
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.charging
            ),
        )

        await asyncio.sleep(10)
        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.available
            ),
        )

    @pytest.mark.asyncio
    @pytest.mark.everest_core_config(
        get_everest_config_path_str("everest-config-sil-iso no-tls.yaml")
    )
    async def test_eim_02(
        self,
        charge_point_v16: ChargePoint16,
        test_controller: TestController,
        test_utility: TestUtility,
    ):
        """
        Test normal EIM authentication with plugin first and autocharge.
        We should test that:
            - Charging process starts
            - DataTransfer(Authorize.req) is not transmitted in this case.
        """

        # plug in
        test_controller.plug_in_ac_iso()

        # expect StartTransaction.req
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StartTransaction",
            call.StartTransactionPayload(1, None, 0, ""),
            validate_standard_start_transaction,
        )

        start_transaction_req = test_utility.messages.pop()
        assert "VID" in start_transaction_req.payload["idTag"]

        # expect StatusNotification with status charging
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.charging
            ),
        )

        await asyncio.sleep(10)
        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.available
            ),
        )

    @pytest.mark.asyncio
    async def test_pnc_reject(
        self,
        test_config,
        central_system_v16: CentralSystem,
        charge_point_v16: ChargePoint16,
        test_controller: TestController,
        test_utility: TestUtility,
    ):
        """
        CSMS rejects DataTransfer(Authorize.req) from CP
        Charging process should not start, even when a valid RFID is presented.
        """

        setattr(charge_point_v16, "on_data_transfer", on_data_transfer_reject_authorize)
        central_system_v16.chargepoint.route_map = create_route_map(
            central_system_v16.chargepoint
        )

        test_utility.forbidden_actions.append("StartTransaction")

        # plug in first
        test_controller.plug_in_ac_iso()

        # expect StatusNotification with status preparing
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
            ),
        )

        # expect authorize.req
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "DataTransfer",
            {
                "messageId": "Authorize",
                "vendorId": "org.openchargealliance.iso15118pnc",
            },
        )

        test_utility.messages.clear()

        # swipe second
        test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "StatusNotification",
            call.StatusNotificationPayload(
                1, ChargePointErrorCode.no_error, ChargePointStatus.available
            ),
        )

    @pytest.mark.asyncio
    async def test_eim_and_autocharge(self, charge_point_v16: ChargePoint16):
        pass

    @pytest.mark.asyncio
    async def test_eim_only(self, charge_point_v16: ChargePoint16):
        pass

    @pytest.mark.asyncio
    async def test_pnc_certificate_signed_01(self, charge_point_v16: ChargePoint16):
        """
        Test with invalid certificate chain
        """
        certificate_signed_req = {"certificateChain": "InvalidCertificateChain"}
        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="CertificateSigned",
            data=json.dumps(certificate_signed_req),
        )

        assert json.loads(data_transfer_response.data) == {"status": "Rejected"}
        assert data_transfer_response.status == "Accepted"

    @pytest.mark.asyncio
    async def test_pnc_delete_certificate(self, charge_point_v16: ChargePoint16):
        """
        Test delete certificate. Test with valid and invalid CertificateHashData
        """
        certificate_hash_data = {
            "hashAlgorithm": "SHA256",
            "issuerKeyHash": "XXXXXXXXXXXXXXXXXXXXXXXXXXXX",
            "issuerNameHash": "YYYYYYYYYYYYYYYYYYYYYYYYYYY",
            "serialNumber": "1",
        }
        delete_certificate_req = {"certificateHashData": certificate_hash_data}

        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="DeleteCertificate",
            data=json.dumps(delete_certificate_req),
        )

        # expect not found
        assert json.loads(data_transfer_response.data) == {"status": "NotFound"}
        assert data_transfer_response.status == "Accepted"

        certificate_hash_data["hashAlgorithm"] = "SHA_Invalid"
        delete_certificate_req = {"certificateHashData": certificate_hash_data}
        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="DeleteCertificate",
            data=json.dumps(delete_certificate_req),
        )

        # expect rejected because of invalid hash algorithm
        assert data_transfer_response.status == "Rejected"

    @pytest.mark.asyncio
    async def test_pnc_get_15118_ev_certificate(self):
        pass

    @pytest.mark.asyncio
    @pytest.mark.skip("Test does nothing yet")
    async def test_pnc_get_certificate_status(self, charge_point_v16: ChargePoint16):
        await asyncio.sleep(30)

    @pytest.mark.asyncio
    async def test_pnc_get_installed_certificate_ids(
        self, charge_point_v16: ChargePoint16
    ):
        """
        Test get installed certificate ids. Test with valid and invalid request
        """
        get_installed_certificate_ids_req = {"certificateType": ["MORootCertificate"]}
        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="GetInstalledCertificateIds",
            data=json.dumps(get_installed_certificate_ids_req),
        )

        assert "status" in json.loads(data_transfer_response.data)

        get_installed_certificate_ids_req = {
            "certificateType": ["ManufacturerRootCertificate"]
        }

        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="GetInstalledCertificateIds",
            data=json.dumps(get_installed_certificate_ids_req),
        )

        assert "status" in json.loads(data_transfer_response.data)

        get_installed_certificate_ids_req = {
            "certificateType": ["InvalidRootCertificateType"]
        }

        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="GetInstalledCertificateIds",
            data=json.dumps(get_installed_certificate_ids_req),
        )

        assert data_transfer_response.status == "Rejected"

    @pytest.mark.asyncio
    @pytest.mark.source_certs_dir(Path(__file__).parent.parent / "everest-aux")
    async def test_pnc_get_installed_certificate_ids_empty_not_found(
        self, charge_point_v16: ChargePoint16
    ):
        """
        Test get installed certificate ids when no certificates are installed.
        """
        get_installed_certificate_ids_req = {"certificateType": []}
        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="GetInstalledCertificateIds",
            data=json.dumps(get_installed_certificate_ids_req),
        )

        assert data_transfer_response.status == "Accepted"
        assert json.loads(data_transfer_response.data) == {"status": "NotFound"}
        

    @pytest.mark.asyncio
    async def test_pnc_install_certificate(
        self, request, charge_point_v16: ChargePoint16
    ):

        v2g_root_ca_path = (
            Path(request.config.getoption("--everest-prefix"))
            / "etc/everest/certs/ca/v2g/V2G_ROOT_CA.pem"
        )
        try:
            os.remove(v2g_root_ca_path)
        except Exception:
            print(f"Could not remove dir: {v2g_root_ca_path}")

        with open(
            Path(os.path.dirname(__file__)).parent
            / "everest-aux/certs/ca/v2g/V2G_ROOT_CA.pem",
            "r",
        ) as f:
            v2g_certificate_chain = f.read()

        install_certificate_req = {
            "certificateType": "V2GRootCertificate",
            "certificate": v2g_certificate_chain,
        }

        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="InstallCertificate",
            data=json.dumps(install_certificate_req),
        )

        assert data_transfer_response.status == "Accepted"
        assert json.loads(data_transfer_response.data) == {"status": "Accepted"}

        install_certificate_req["certificate"] = "InvalidCertificate"

        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="InstallCertificate",
            data=json.dumps(install_certificate_req),
        )

        assert data_transfer_response.status == "Accepted"
        assert json.loads(data_transfer_response.data) == {"status": "Rejected"}

    @pytest.mark.asyncio
    async def test_pnc_sign_certificate_and_trigger_message(
        self, test_utility, charge_point_v16: ChargePoint16
    ):
        trigger_message_req = {"requestedMessage": "SignCertificate"}

        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="TriggerMessage",
            data=json.dumps(trigger_message_req),
        )

        assert data_transfer_response.status == "Accepted"
        assert json.loads(data_transfer_response.data) == {"status": "Accepted"}

        csr = await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "DataTransfer",
            call.DataTransferPayload(
                data=json.dumps(
                    remove_nones(
                        snake_to_camel_case(
                            asdict(
                                call201.SignCertificatePayload(
                                    csr="",
                                    certificate_type=CertificateSigningUseType.v2g_certificate,
                                )
                            )
                        )
                    ),
                    separators=(",", ":"),
                ),
                message_id="SignCertificate",
                vendor_id="org.openchargealliance.iso15118pnc",
            ),
            validate_data_transfer_sign_certificate,
        )

        assert csr
        certificate_signed = certificate_signed_response(csr)
        certificate_signed_req = {"certificateChain": certificate_signed}

        data_transfer_response = await charge_point_v16.data_transfer_req(
            vendor_id="org.openchargealliance.iso15118pnc",
            message_id="CertificateSigned",
            data=json.dumps(certificate_signed_req),
        )

        assert json.loads(data_transfer_response.data) == {"status": "Accepted"}
        assert data_transfer_response.status == "Accepted"


@pytest.mark.asyncio
@pytest.mark.ocpp_config_adaptions(
        GenericOCPP16ConfigAdjustment(
            [("Internal", "ConnectorEvseIds", "test_value"),
             ("Internal", "SeccLeafSubjectCommonName", "DEPNX100001"),
             ("Internal", "SeccLeafSubjectOrganization", "test_value"),
             ("Internal", "SeccLeafSubjectCountry", "DE")]
        )
    )
async def test_set_connector_evse_ids(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    
    initial_value = "test_value"
    invalid_value = "WRONG,DE*PNX*100001"
    new_valid_value = "DE*PNX*100001,DE*PNX*100002"

    await charge_point_v16.change_configuration_req(
        key="ConnectorEvseIds", value=invalid_value
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.rejected),
    )

    await charge_point_v16.get_configuration_req(key=["ConnectorEvseIds"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [{"key": "ConnectorEvseIds", "readonly": False, "value": initial_value}]
        ),
    )

    await charge_point_v16.change_configuration_req(
        key="ConnectorEvseIds", value=new_valid_value
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    await charge_point_v16.get_configuration_req(key=["ConnectorEvseIds"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [{"key": "ConnectorEvseIds", "readonly": False, "value": new_valid_value}]
        ),
    )

    test_utility.messages.clear()

    long_value = "waaaaaaaaaaaaaaaaaaaaaaaaaaaaaay_too_looooooooooooooooooooooooooooooooooong"
    short_value = "short"


    await charge_point_v16.change_configuration_req(
        key="SeccLeafSubjectCommonName", value=short_value
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.rejected),
    )
    test_utility.messages.clear()

    await charge_point_v16.change_configuration_req(
        key="SeccLeafSubjectCommonName", value=long_value
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.rejected),
    )
    test_utility.messages.clear()

    await charge_point_v16.change_configuration_req(
        key="SeccLeafSubjectOrganization", value=long_value
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.rejected),
    )
    test_utility.messages.clear()

    await charge_point_v16.change_configuration_req(
        key="SeccLeafSubjectCountry", value="GER"
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.rejected),
    )
    test_utility.messages.clear()

    await charge_point_v16.change_configuration_req(
        key="SeccLeafSubjectCommonName", value=short_value
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.rejected),
    )

    # valid values
    valid_common_name = "valid_common_name"
    valid_organization = "valid_organization"
    valid_country = "GB"

    test_utility.messages.clear()

    await charge_point_v16.change_configuration_req(
        key="SeccLeafSubjectCommonName", value=valid_common_name
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )
    test_utility.messages.clear()

    await charge_point_v16.change_configuration_req(
        key="SeccLeafSubjectOrganization", value=valid_organization
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )
    test_utility.messages.clear()

    await charge_point_v16.change_configuration_req(
        key="SeccLeafSubjectCountry", value=valid_country
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )
    test_utility.messages.clear()

    await charge_point_v16.get_configuration_req(key=["ConnectorEvseIds", "SeccLeafSubjectCommonName",
                                                      "SeccLeafSubjectOrganization", "SeccLeafSubjectCountry"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [{"key": "ConnectorEvseIds", "readonly": False, "value": new_valid_value},
             {"key": "SeccLeafSubjectCommonName", "readonly": False, "value": valid_common_name},
             {"key": "SeccLeafSubjectOrganization", "readonly": False, "value": valid_organization},
             {"key": "SeccLeafSubjectCountry", "readonly": False, "value": valid_country}]
        ),
    )

    

