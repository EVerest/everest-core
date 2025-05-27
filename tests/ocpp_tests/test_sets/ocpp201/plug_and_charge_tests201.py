# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

# fmt: off
import os
import sys

from everest.testing.core_utils.controller.test_controller_interface import TestController

sys.path.append(os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../..")))
from everest.testing.ocpp_utils.fixtures import *
from ocpp.v201.enums import DeleteCertificateStatusType, AuthorizeCertificateStatusType
from ocpp.v201 import call as call201
from ocpp.routing import create_route_map
import asyncio
import pytest
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP201ConfigAdjustment
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
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201-sil-dc-d2.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP201ConfigAdjustment(
        [
            (
                OCPP201ConfigVariableIdentifier(
                    "ISO15118Ctrlr",
                    "ContractCertificateInstallationEnabled",
                    "Actual",
                ),
                True,
            )
        ]
    )
)
class TestPlugAndCharge:

    @pytest.mark.asyncio
    @pytest.mark.source_certs_dir(Path(__file__).parent.parent / "everest-aux/certs")
    async def test_contract_installation_and_authorization_01(
        self,
        request,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
        test_controller: TestController,
        test_config,
        test_utility: TestUtility,
    ):
        """
        Test for contract installation on the vehicle and succeeding authorization and charging process
        """

        setattr(
            charge_point, "on_get_15118_ev_certificate", on_get_15118_ev_certificate
        )
        central_system.chargepoint.route_map = create_route_map(
            central_system.chargepoint
        )

        test_controller.plug_in_dc_iso()

        assert await wait_for_and_validate(
            test_utility, charge_point, "Get15118EVCertificate", {"action": "Install"}
        )

        # expect authorize.req
        authorize_req: call201.AuthorizePayload = call201.AuthorizePayload(
            **await wait_for_and_validate(test_utility, charge_point, "Authorize", {})
        )

        assert validate_authorize_req(authorize_req, False, True)

        # expect StartTransaction.req
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "TransactionEvent",
            {
                "eventType": "Started",
                "id_token": {
                    "idToken": test_config.authorization_info.emaid,
                    "type": "eMAID",
                },
            },
        )

        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "StatusNotification",
            {"connectorStatus": "Available"},
        )

    @pytest.mark.asyncio
    async def test_contract_installation_and_authorization_02(
        self,
        request,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
        test_controller: TestController,
        test_utility: TestUtility,
    ):
        """
        Test for contract installation on the vehicle and succeeding authorization request that is rejected by CSMS
        """

        @on(Action.Authorize)
        def on_authorize(**kwargs):
            return call_result201.AuthorizePayload(
                id_token_info=IdTokenInfoType(
                    status=AuthorizationStatusType.blocked,
                )
            )

        setattr(
            charge_point, "on_get_15118_ev_certificate", on_get_15118_ev_certificate
        )
        setattr(charge_point, "on_authorize", on_authorize)

        central_system.chargepoint.route_map = create_route_map(
            central_system.chargepoint
        )

        await asyncio.sleep(3)
        test_controller.plug_in_dc_iso()

        assert await wait_for_and_validate(
            test_utility, charge_point, "Get15118EVCertificate", {"action": "Install"}
        )

        # expect authorize.req
        authorize_req: call201.AuthorizePayload = call201.AuthorizePayload(
            **await wait_for_and_validate(test_utility, charge_point, "Authorize", {})
        )

        assert validate_authorize_req(authorize_req, False, True)

        test_utility.messages.clear()
        test_utility.forbidden_actions.append("StartTransaction")

        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "StatusNotification",
            {"connectorStatus": "Available"},
        )

    @pytest.mark.asyncio
    @pytest.mark.source_certs_dir(Path(__file__).parent.parent / "everest-aux/certs")
    @pytest.mark.ocpp_config_adaptions(
        GenericOCPP201ConfigAdjustment(
            [
                (
                    OCPP201ConfigVariableIdentifier(
                        "ISO15118Ctrlr",
                        "CentralContractValidationAllowed",
                        "Actual",
                    ),
                    True,
                ),
                (
                    OCPP201ConfigVariableIdentifier(
                        "ISO15118Ctrlr",
                        "ContractCertificateInstallationEnabled",
                        "Actual",
                    ),
                    True,
                ),
            ]
        )
    )
    async def test_contract_installation_and_authorization_03(
        self,
        request,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
        test_controller: TestController,
        test_config,
        test_utility: TestUtility,
    ):
        """
        Test for contract installation on the vehicle and succeeding authorization and charging process
        """

        setattr(
            charge_point, "on_get_15118_ev_certificate", on_get_15118_ev_certificate
        )
        central_system.chargepoint.route_map = create_route_map(
            central_system.chargepoint
        )

        certificate_hash_data = {
            "hashAlgorithm": "SHA256",
            "issuerKeyHash": "66fce9295edc049f4a183458948ecaa8e3558e4aa3041f13a2363d1d953d33e5",
            "issuerNameHash": "3a1ad85a129bd5db30c2f099a541f76e562b8a30e9f49f3f47077eeae3750a2a",
            "serialNumber": "3041",
        }

        delete_certificate_req = {"certificate_hash_data": certificate_hash_data}

        # delete MO root
        delete_certificate_response: call_result201.DeleteCertificatePayload = (
            await charge_point.delete_certificate_req(
                **delete_certificate_req,
            )
        )

        assert (
            delete_certificate_response.status == DeleteCertificateStatusType.accepted
        )

        test_controller.plug_in_dc_iso()

        assert await wait_for_and_validate(
            test_utility, charge_point, "Get15118EVCertificate", {"action": "Install"}
        )

        test_utility.messages.clear()

        # expect authorize.req
        authorize_req: call201.AuthorizePayload = call201.AuthorizePayload(
            **await wait_for_and_validate(test_utility, charge_point, "Authorize", {})
        )

        assert validate_authorize_req(authorize_req, True, False)

        # expect StartTransaction.req
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "TransactionEvent",
            {
                "eventType": "Started",
                "id_token": {
                    "idToken": test_config.authorization_info.emaid,
                    "type": "eMAID",
                },
            },
        )

        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "StatusNotification",
            {"connectorStatus": "Available"},
        )

    @pytest.mark.asyncio
    @pytest.mark.source_certs_dir(Path(__file__).parent.parent / "everest-aux/certs")
    @pytest.mark.ocpp_config_adaptions(
        GenericOCPP201ConfigAdjustment(
            [
                (
                    OCPP201ConfigVariableIdentifier(
                        "ISO15118Ctrlr",
                        "CentralContractValidationAllowed",
                        "Actual",
                    ),
                    False,
                ),
                (
                    OCPP201ConfigVariableIdentifier(
                        "ISO15118Ctrlr",
                        "ContractCertificateInstallationEnabled",
                        "Actual",
                    ),
                    True,
                ),
            ]
        )
    )
    async def test_contract_installation_and_authorization_04(
        self,
        request,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
        test_controller: TestController,
        test_config,
        test_utility: TestUtility,
    ):
        """
        Test for contract installation on the vehicle and not succeeding authorization because CentralContractValidationAllowed is false
        """

        setattr(
            charge_point, "on_get_15118_ev_certificate", on_get_15118_ev_certificate
        )
        central_system.chargepoint.route_map = create_route_map(
            central_system.chargepoint
        )

        certificate_hash_data = {
            "hashAlgorithm": "SHA256",
            "issuerKeyHash": "66fce9295edc049f4a183458948ecaa8e3558e4aa3041f13a2363d1d953d33e5",
            "issuerNameHash": "3a1ad85a129bd5db30c2f099a541f76e562b8a30e9f49f3f47077eeae3750a2a",
            "serialNumber": "3041",
        }

        delete_certificate_req = {"certificate_hash_data": certificate_hash_data}

        # delete MO root
        delete_certificate_response: call_result201.DeleteCertificatePayload = (
            await charge_point.delete_certificate_req(
                **delete_certificate_req,
            )
        )

        assert (
            delete_certificate_response.status == DeleteCertificateStatusType.accepted
        )

        test_controller.plug_in_dc_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "StatusNotification",
            {"connectorStatus": "Occupied"},
        )

        assert await wait_for_and_validate(
            test_utility, charge_point, "Get15118EVCertificate", {"action": "Install"}
        )

        test_utility.messages.clear()
        test_utility.forbidden_actions.append("Authorize")
        test_utility.forbidden_actions.append("TransactionEvent")

        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "StatusNotification",
            {"connectorStatus": "Available"},
        )

    @pytest.mark.asyncio
    @pytest.mark.source_certs_dir(Path(__file__).parent.parent / "everest-aux/certs")
    @pytest.mark.ocpp_config_adaptions(
        GenericOCPP201ConfigAdjustment(
            [
                (
                    OCPP201ConfigVariableIdentifier(
                        "ISO15118Ctrlr",
                        "CentralContractValidationAllowed",
                        "Actual",
                    ),
                    True,
                ),
                (
                    OCPP201ConfigVariableIdentifier(
                        "ISO15118Ctrlr",
                        "ContractCertificateInstallationEnabled",
                        "Actual",
                    ),
                    True,
                ),
            ]
        )
    )
    @pytest.mark.asyncio
    async def test_contract_revoked(
        self,
        request,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
        test_controller: TestController,
        test_utility: TestUtility,
    ):
        """
        Test for contract installation on the vehicle and succeeding authorization request that is rejected by CSMS
        """

        @on(Action.Authorize)
        def on_authorize(**kwargs):
            return call_result201.AuthorizePayload(
                id_token_info=IdTokenInfoType(status=AuthorizationStatusType.blocked),
                certificate_status=AuthorizeCertificateStatusType.certificate_revoked,
            )

        setattr(
            charge_point, "on_get_15118_ev_certificate", on_get_15118_ev_certificate
        )
        setattr(charge_point, "on_authorize", on_authorize)

        central_system.chargepoint.route_map = create_route_map(
            central_system.chargepoint
        )

        certificate_hash_data = {
            "hashAlgorithm": "SHA256",
            "issuerKeyHash": "66fce9295edc049f4a183458948ecaa8e3558e4aa3041f13a2363d1d953d33e5",
            "issuerNameHash": "3a1ad85a129bd5db30c2f099a541f76e562b8a30e9f49f3f47077eeae3750a2a",
            "serialNumber": "3041",
        }

        delete_certificate_req = {"certificate_hash_data": certificate_hash_data}

        # delete MO root
        delete_certificate_response: call_result201.DeleteCertificatePayload = (
            await charge_point.delete_certificate_req(
                **delete_certificate_req,
            )
        )

        assert (
            delete_certificate_response.status == DeleteCertificateStatusType.accepted
        )

        await asyncio.sleep(3)
        test_controller.plug_in_dc_iso()

        assert await wait_for_and_validate(
            test_utility, charge_point, "Get15118EVCertificate", {"action": "Install"}
        )

        # expect authorize.req
        authorize_req: call201.AuthorizePayload = call201.AuthorizePayload(
            **await wait_for_and_validate(
                test_utility,
                charge_point,
                "Authorize",
                {"idToken": {"idToken": "UKSWI123456789A", "type": "eMAID"}},
            )
        )

        assert validate_authorize_req(authorize_req, True, False)

        test_utility.messages.clear()
        # verify that transaction does not start
        test_utility.forbidden_actions.append("TransactionEvent")

        test_utility.messages.clear()
        test_controller.plug_out_iso()

        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "StatusNotification",
            {"connectorStatus": "Available"},
        )
