# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import asyncio
import logging
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Any

import pytest
from OpenSSL import crypto
from cryptography import x509
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives._serialization import PublicFormat, Encoding
from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.probe_module import ProbeModule

from everest_test_utils import OCPPConfigReader, CertificateHelper

from unittest.mock import call as mock_call, Mock, ANY
from ocpp.v201 import call_result
from ocpp.v201.datatypes import SetVariableResultType
from ocpp.v201.enums import SetVariableStatusType

from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.ocpp_utils.central_system import CentralSystem
from everest.testing.core_utils._configuration.libocpp_configuration_helper import (
    GenericOCPP201ConfigAdjustment,
    OCPP201ConfigVariableIdentifier,
)
from everest.testing.core_utils._configuration.libocpp_configuration_helper import (
    _OCPP201NetworkConnectionProfileAdjustment,
)


log = logging.getLogger("OCPP201Security")


@dataclass
class _CertificateSigningTestData:
    signed_certificate: str | None = None
    csr: str | None = None
    signed_certificate_valid: bool = True
    csr_accepted: bool = True


class _BaseTest:
    @staticmethod
    async def _wait_for_mock_called(mock, call=None, timeout=2):
        async def _await_called():
            while not mock.call_count or (call and call not in mock.mock_calls):
                await asyncio.sleep(0.1)

        await asyncio.wait_for(_await_called(), timeout=timeout)

    def _setup_csms_mock(self, csms_mock: Mock, test_data: _CertificateSigningTestData):
        status = "Accepted" if test_data.csr_accepted else "Rejected"
        csms_mock.on_sign_certificate.side_effect = (
            lambda csr: call_result.SignCertificatePayload(status=status)
        )

    def _get_expected_csr_data(
        self, certificate_type: str, ocpp_config_reader: OCPPConfigReader
    ):
        if certificate_type == "ChargingStationCertificate":

            return {
                "certificate_type": "CSMS",
                "common": ocpp_config_reader.get_variable(
                    "InternalCtrlr", "ChargeBoxSerialNumber"
                ),
                "country": ocpp_config_reader.get_variable(
                    "ISO15118Ctrlr", "ISO15118CtrlrCountryName"
                ),
                "organization": ocpp_config_reader.get_variable(
                    "SecurityCtrlr", "OrganizationName"
                ),
                "use_tpm": False,
            }
        else:
            return {
                "certificate_type": "V2G",
                "common": ocpp_config_reader.get_variable(
                    "InternalCtrlr", "ChargeBoxSerialNumber"
                ),
                "country": ocpp_config_reader.get_variable(
                    "ISO15118Ctrlr", "ISO15118CtrlrCountryName"
                ),
                "organization": ocpp_config_reader.get_variable(
                    "ISO15118Ctrlr", "ISO15118CtrlrOrganizationName"
                ),
                "use_tpm": False,
            }


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config("everest-config-ocpp201-probe-module.yaml")
@pytest.mark.inject_csms_mock
@pytest.mark.probe_module
@pytest.mark.parametrize(
    "skip_implementation",
    [
        {
            "ProbeModuleSecurity": [
                "generate_certificate_signing_request",
                "update_leaf_certificate",
            ]
        }
    ],
)
@pytest.mark.skip("The certificate chains are not properly set up to run these tests")
class TestSecurityOCPPIntegration(_BaseTest):
    @dataclass
    class _SecurityModuleMocks:
        generate_certificate_signing_request: Mock
        update_leaf_certificate: Mock

    def _setup_security_module_mocks(
        self, probe_module: ProbeModule, test_data: _CertificateSigningTestData
    ) -> _SecurityModuleMocks:
        security_generate_certificate_signing_request_mock = Mock()
        security_generate_certificate_signing_request_mock.side_effect = (
            lambda arg: test_data.csr
        )
        probe_module.implement_command(
            "ProbeModuleSecurity",
            "generate_certificate_signing_request",
            security_generate_certificate_signing_request_mock,
        )

        security_update_leaf_certificate_mock = Mock()
        security_update_leaf_certificate_mock.side_effect = lambda arg: (
            "Accepted"
            if test_data.signed_certificate_valid
            else "InvalidCertificateChain"
        )
        probe_module.implement_command(
            "ProbeModuleSecurity",
            "update_leaf_certificate",  # installs and verifies
            security_update_leaf_certificate_mock,
        )

        return self._SecurityModuleMocks(
            generate_certificate_signing_request=security_generate_certificate_signing_request_mock,
            update_leaf_certificate=security_update_leaf_certificate_mock,
        )

    @pytest.mark.parametrize(
        "certificate_type", ["ChargingStationCertificate", "V2GCertificate"]
    )
    async def test_A02_update_charging_station_certificate_by_csms_request(
        self,
        certificate_type,
        probe_module,
        ocpp_config_reader,
        central_system: CentralSystem,
    ):
        """A02 use case success behavior."""

        # Setup Test Data  & mocks
        csms_mock = central_system.mock
        test_data = _CertificateSigningTestData(
            csr="mock certificate request", signed_certificate="mock signed certificate"
        )
        security_mocks = self._setup_security_module_mocks(probe_module, test_data)
        self._setup_csms_mock(csms_mock, test_data)

        # start and ready probe module EvseManagers and wait for libocpp to connect
        probe_module.start()
        await probe_module.wait_to_be_ready()
        probe_module.publish_variable("ProbeModuleConnectorA", "ready", True)
        probe_module.publish_variable("ProbeModuleConnectorB", "ready", True)
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # # Act CSMS triggers SignChargingStationCertificate
        trigger_result: call_result.TriggerMessagePayload = (
            await chargepoint_with_pm.trigger_message_req(
                requested_message=f"Sign{certificate_type}"  # todo: SignV2GCertificate
            )
        )

        # Verify:
        #  - OCPP accepts trigger and
        #  - calls security module "generate_certificate_signing_request"
        #  - sends CSR to CSMS
        assert trigger_result.status == "Accepted"
        await self._wait_for_mock_called(
            security_mocks.generate_certificate_signing_request
        )
        assert security_mocks.generate_certificate_signing_request.mock_calls == [
            mock_call(self._get_expected_csr_data(certificate_type, ocpp_config_reader))
        ]
        #
        await self._wait_for_mock_called(csms_mock.on_sign_certificate)
        assert csms_mock.on_sign_certificate.mock_calls == [
            mock_call(csr=test_data.csr)
        ]

        # Act II: CSMS sends signed result to ChargePoint
        signed_result: call_result.CertificateSignedPayload = (
            await chargepoint_with_pm.certificate_signed_req(
                certificate_chain=test_data.signed_certificate,
                certificate_type=certificate_type,
            )
        )

        # Verify II
        # -  Chargepoint accepts signed certificate
        #  - OCPP module verifies and installs certificate in Security Module
        assert signed_result == call_result.CertificateSignedPayload(status="Accepted")
        await self._wait_for_mock_called(security_mocks.update_leaf_certificate)
        assert security_mocks.update_leaf_certificate.mock_calls == [
            mock_call(
                {
                    "certificate_chain": test_data.signed_certificate,
                    "certificate_type": (
                        "CSMS"
                        if certificate_type == "ChargingStationCertificate"
                        else "V2G"
                    ),
                }
            )
        ]

    async def test_A02_update_charging_station_certificate_by_csms_request_retry(
        self, probe_module, ocpp_config_reader, central_system: CentralSystem
    ):
        """Test the retry behavior on failed attempts.

        In particular tests requirements  A02.FR.17, A02.FR.18, A02.FR.19"""

        # Setup Test Data  & mocks
        csms_mock = central_system.mock
        test_data = _CertificateSigningTestData(
            csr="mock certificate request", signed_certificate="mock signed certificate"
        )
        certificate_type = "ChargingStationCertificate"
        security_mocks = self._setup_security_module_mocks(probe_module, test_data)
        self._setup_csms_mock(csms_mock, test_data)

        # start and ready probe module EvseManagers and wait for libocpp to connect
        probe_module.start()
        await probe_module.wait_to_be_ready()
        probe_module.publish_variable("ProbeModuleConnectorA", "ready", True)
        probe_module.publish_variable("ProbeModuleConnectorB", "ready", True)
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Act CSMS triggers SignChargingStationCertificate
        trigger_result: call_result.TriggerMessagePayload = (
            await chargepoint_with_pm.trigger_message_req(
                requested_message="SignChargingStationCertificate"
            )
        )

        # Verify:
        #  - OCPP accepts trigger and
        #  - calls security module "generate_certificate_signing_request"
        #  - sends CSR to CSMS
        assert trigger_result.status == "Accepted"
        await self._wait_for_mock_called(
            security_mocks.generate_certificate_signing_request
        )
        assert security_mocks.generate_certificate_signing_request.mock_calls == [
            mock_call(self._get_expected_csr_data(certificate_type, ocpp_config_reader))
        ]

        await self._wait_for_mock_called(csms_mock.on_sign_certificate)
        assert csms_mock.on_sign_certificate.mock_calls == [
            mock_call(csr=test_data.csr)
        ]

        # Verify: The CSMS does not send the certificate, thus request is repeated as many times as configured

        repeat_times = ocpp_config_reader.get_variable(
            "SecurityCtrlr", "CertSigningRepeatTimes"
        )

        async def _await_called():
            while not len(csms_mock.on_sign_certificate.mock_calls) == repeat_times:
                await asyncio.sleep(0.1)

        await asyncio.wait_for(_await_called(), 10)
        await asyncio.sleep(0.1)  # await unexpected further call

        assert csms_mock.on_sign_certificate.mock_calls == repeat_times * [
            mock_call(csr=test_data.csr)
        ]
        security_mocks.update_leaf_certificate.assert_not_called()

    async def test_A04_rejected_security_event_notification(
        self, probe_module, ocpp_config_reader, central_system: CentralSystem
    ):
        """A02 & A04:  OCPP module sends security event if  certificate is rejected

        Also tests A02.FR.20 (no repetition)
        """

        # Setup Test Data  & mocks
        csms_mock = central_system.mock
        test_data = _CertificateSigningTestData(
            csr="mock certificate request",
            signed_certificate="mock signed certificate",
            signed_certificate_valid=False,
        )
        certificate_type = "ChargingStationCertificate"
        security_mocks = self._setup_security_module_mocks(probe_module, test_data)
        self._setup_csms_mock(csms_mock, test_data)

        # start and ready probe module EvseManagers and wait for libocpp to connect
        probe_module.start()
        await probe_module.wait_to_be_ready()
        probe_module.publish_variable("ProbeModuleConnectorA", "ready", True)
        probe_module.publish_variable("ProbeModuleConnectorB", "ready", True)
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Act CSMS triggers SignChargingStationCertificate
        trigger_result: call_result.TriggerMessagePayload = (
            await chargepoint_with_pm.trigger_message_req(
                requested_message="SignChargingStationCertificate"  # todo: SignV2GCertificate
            )
        )

        # Verify:
        #  - OCPP accepts trigger and
        #  - calls security module "generate_certificate_signing_request"
        #  - sends CSR to CSMS
        assert trigger_result.status == "Accepted"
        await self._wait_for_mock_called(
            security_mocks.generate_certificate_signing_request
        )
        assert security_mocks.generate_certificate_signing_request.mock_calls == [
            mock_call(self._get_expected_csr_data(certificate_type, ocpp_config_reader))
        ]

        await self._wait_for_mock_called(csms_mock.on_sign_certificate)
        assert csms_mock.on_sign_certificate.mock_calls == [
            mock_call(csr=test_data.csr)
        ]

        # Act II: CSMS sends signed result to ChargePoint
        signed_result: call_result.CertificateSignedPayload = (
            await chargepoint_with_pm.certificate_signed_req(
                certificate_chain=test_data.signed_certificate,
                certificate_type=certificate_type,
            )
        )
        # Verify II
        # -  Chargepoint accepts signed certificate
        #  - OCPP module rejects certificate and sends security event notification
        assert signed_result == call_result.CertificateSignedPayload(status="Rejected")
        await self._wait_for_mock_called(
            csms_mock.on_security_event_notification,
            call=mock_call(
                timestamp=ANY,
                tech_info="InvalidCertificateChain",
                type="InvalidChargingStationCertificate",
            ),
        )

        # test A02.FR.20 - no repeated request
        await asyncio.sleep(
            0.3
        )  # wait the minimum time between two retries and a little more
        assert csms_mock.on_sign_certificate.call_count == 1


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config("everest-config-ocpp201.yaml")
@pytest.mark.source_certs_dir(Path(__file__).parent.parent / "everest-aux/certs")
@pytest.mark.inject_csms_mock
@pytest.mark.skip("The certificate chains are not properly set up to run these tests")
class TestSecurityOCPPE2E(_BaseTest):
    @dataclass
    class _ParsedCSR:
        csr: str
        common: str
        organization: str
        country: str
        email_address: str | None
        public_key: str

    @classmethod
    def _parse_certificate_request(cls, csr: str) -> _ParsedCSR:
        request = x509.load_pem_x509_csr(csr.encode("utf-8"), default_backend())
        email_address = None
        if request.subject.get_attributes_for_oid(x509.NameOID.EMAIL_ADDRESS):
            email_address = request.subject.get_attributes_for_oid(
                x509.NameOID.EMAIL_ADDRESS
            )[0].value
        return cls._ParsedCSR(
            csr,
            request.subject.get_attributes_for_oid(x509.NameOID.COMMON_NAME)[0].value,
            request.subject.get_attributes_for_oid(x509.NameOID.ORGANIZATION_NAME)[
                0
            ].value,
            request.subject.get_attributes_for_oid(x509.NameOID.COUNTRY_NAME)[0].value,
            email_address,
            request.public_key()
            .public_bytes(
                encoding=Encoding.PEM, format=PublicFormat.SubjectPublicKeyInfo
            )
            .decode("utf-8"),
        )

    @pytest.mark.parametrize(
        "certificate_type", ["ChargingStationCertificate", "V2GCertificate"]
    )
    async def test_A02_update_charging_station_certificate_by_csms_request(
        self,
        certificate_type,
        ocpp_config_reader,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
    ):
        """A02 use case success behavior (installation of new certificate)

        Tested Requirements: A02.FR.02, A02.FR.05 (as far as possible in this context), A02.FR.06
        """

        # Setup Test Data  & mocks
        csms_mock = central_system.mock
        test_data = _CertificateSigningTestData(
            signed_certificate="mock signed certificate"
        )

        self._setup_csms_mock(csms_mock, test_data)

        # # Act CSMS triggers SignChargingStationCertificate
        trigger_result: call_result.TriggerMessagePayload = (
            await charge_point.trigger_message_req(
                requested_message=f"Sign{certificate_type}"
            )
        )

        # Verify:
        #  - OCPP accepts trigger and
        #  - calls security module "generate_certificate_signing_request"
        #  - sends CSR to CSMS
        assert trigger_result.status == "Accepted"

        await self._wait_for_mock_called(csms_mock.on_sign_certificate)
        assert csms_mock.on_sign_certificate.mock_calls == [
            mock_call(csr=ANY, certificate_type=certificate_type)
        ]
        received_csr_data = self._parse_certificate_request(
            csms_mock.on_sign_certificate.mock_calls[0].kwargs["csr"]
        )
        expected_csr_data = self._get_expected_csr_data(
            certificate_type, ocpp_config_reader
        )
        assert received_csr_data.common == expected_csr_data["common"]
        assert received_csr_data.organization == expected_csr_data["organization"]
        assert received_csr_data.country == expected_csr_data["country"]

        if certificate_type == "ChargingStationCertificate":
            signed_certificate = CertificateHelper.sign_certificate_request(
                received_csr_data.csr,
                issuer_certificate_path=Path(__file__).parent
                / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.pem",
                issuer_private_key_path=Path(__file__).parent
                / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.key",
                issuer_private_key_passphrase="123456",
            )
        else:
            # build certificate chain starting with leaf up to CPO SUB CA1
            signed_certificate = CertificateHelper.sign_certificate_request(
                received_csr_data.csr,
                issuer_certificate_path=Path(__file__).parent
                / "../everest-aux/certs/ca/cso/CPO_SUB_CA2.pem",
                issuer_private_key_path=Path(__file__).parent
                / "../everest-aux/certs/client/csms/CPO_SUB_CA2.key",
                issuer_private_key_passphrase="123456",
            )
            signed_certificate += (
                Path(__file__).parent / "../everest-aux/certs/ca/cso/CPO_SUB_CA2.pem"
            ).read_text() + "\n"
            signed_certificate += (
                Path(__file__).parent / "../everest-aux/certs/ca/cso/CPO_SUB_CA1.pem"
            ).read_text() + "\n"

        # Act II: CSMS sends signed result to ChargePoint
        signed_result: call_result.CertificateSignedPayload = (
            await charge_point.certificate_signed_req(
                certificate_chain=signed_certificate, certificate_type=certificate_type
            )
        )

        # Verify II
        # -  Chargepoint accepts signed certificate
        #  - OCPP module verifies and installs certificate in Security Module
        assert signed_result == call_result.CertificateSignedPayload(status="Accepted")

    async def test_A02_update_charging_station_certificate_by_csms_request_invalid_as_expired(
        self,
        ocpp_config_reader,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
    ):
        """Test the charging station rejects an expired certificate after a signing request."""
        csms_mock = central_system.mock
        test_data = _CertificateSigningTestData(
            signed_certificate="mock signed certificate"
        )

        self._setup_csms_mock(csms_mock, test_data)

        # # Act CSMS triggers SignChargingStationCertificate
        trigger_result: call_result.TriggerMessagePayload = (
            await charge_point.trigger_message_req(
                requested_message=f"SignChargingStationCertificate"
            )
        )

        # Verify:
        #  - OCPP accepts trigger and
        #  - calls security module "generate_certificate_signing_request"
        #  - sends CSR to CSMS
        assert trigger_result.status == "Accepted"

        await self._wait_for_mock_called(csms_mock.on_sign_certificate)
        received_csr_data = self._parse_certificate_request(
            csms_mock.on_sign_certificate.mock_calls[0].kwargs["csr"]
        )
        signed_certificate = CertificateHelper.sign_certificate_request(
            received_csr_data.csr,
            issuer_certificate_path=Path(__file__).parent
            / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.pem",
            issuer_private_key_path=Path(__file__).parent
            / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.key",
            issuer_private_key_passphrase="123456",
            relative_expiration_time=-60,  # expired a minute ago
        )

        # Act II: CSMS sends signed result to ChargePoint
        signed_result: call_result.CertificateSignedPayload = (
            await charge_point.certificate_signed_req(
                certificate_chain=signed_certificate,
                certificate_type="ChargingStationCertificate",
            )
        )

        # Verify II
        # -  Chargepoint accepts signed certificate
        #  - OCPP module rejects wrongly signed certificate
        assert signed_result == call_result.CertificateSignedPayload(status="Rejected")

        # Assert an InvalidChargingStationCertificate event is triggered
        await self._wait_for_mock_called(
            csms_mock.on_security_event_notification,
            call=mock_call(
                timestamp=ANY,
                tech_info="Expired",
                type="InvalidChargingStationCertificate",
            ),
        )

    async def test_A02_update_charging_station_certificate_by_csms_request_invalid_due_to_wrong_ca(
        self,
        ocpp_config_reader,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
    ):
        csms_mock = central_system.mock
        test_data = _CertificateSigningTestData(
            signed_certificate="mock signed certificate"
        )

        self._setup_csms_mock(csms_mock, test_data)

        # # Act CSMS triggers SignChargingStationCertificate
        trigger_result: call_result.TriggerMessagePayload = (
            await charge_point.trigger_message_req(
                requested_message=f"SignChargingStationCertificate"
            )
        )

        # Verify:
        #  - OCPP accepts trigger and
        #  - calls security module "generate_certificate_signing_request"
        #  - sends CSR to CSMS
        assert trigger_result.status == "Accepted"

        await self._wait_for_mock_called(csms_mock.on_sign_certificate)
        received_csr_data = self._parse_certificate_request(
            csms_mock.on_sign_certificate.mock_calls[0].kwargs["csr"]
        )
        # the MO root ca is invalid for the CSMS certificate!
        signed_certificate = CertificateHelper.sign_certificate_request(
            received_csr_data.csr,
            issuer_certificate_path=Path(__file__).parent
            / "../everest-aux/certs/ca/mo/MO_ROOT_CA.pem",
            issuer_private_key_path=Path(__file__).parent
            / "../everest-aux/certs/client/mo/MO_ROOT_CA.key",
            issuer_private_key_passphrase="123456",
        )

        # Act II: CSMS sends signed result to ChargePoint
        signed_result: call_result.CertificateSignedPayload = (
            await charge_point.certificate_signed_req(
                certificate_chain=signed_certificate,
                certificate_type="ChargingStationCertificate",
            )
        )

        # Verify II
        # -  Chargepoint accepts signed certificate
        #  - OCPP module rejects wrongly signed certificate
        assert signed_result == call_result.CertificateSignedPayload(status="Rejected")

        # Assert an InvalidChargingStationCertificate event is triggered
        await self._wait_for_mock_called(
            csms_mock.on_security_event_notification,
            call=mock_call(
                timestamp=ANY,
                tech_info="InvalidCertificateChain",
                type="InvalidChargingStationCertificate",
            ),
        )

    def assert_websocket_client_sslproto_certificate_equals_certificate(
        self, websocket_client_cert: dict[str, Any], certificate: str
    ):
        x509_cert = crypto.load_certificate(
            crypto.FILETYPE_PEM, certificate.encode("utf-8")
        )

        def _compare_websocket_and_cert_components(
            websocket_components, cert_components
        ):
            websocket_cert_subject_dict = {k: v for ((k, v),) in websocket_components}
            cert_subject_dict = {k: v for (k, v) in cert_components}
            for websocket_key, cert_key in [
                ("countryName", b"C"),
                ("commonName", b"CN"),
                ("organizationName", b"O"),
                ("domainComponent", b"DC"),
            ]:
                assert websocket_cert_subject_dict.get(
                    websocket_key, ""
                ) == cert_subject_dict.get(cert_key, b"").decode("utf-8")

        _compare_websocket_and_cert_components(
            websocket_client_cert["subject"], x509_cert.get_subject().get_components()
        )
        _compare_websocket_and_cert_components(
            websocket_client_cert["issuer"], x509_cert.get_issuer().get_components()
        )
        assert (
            int(websocket_client_cert["serialNumber"], 16)
            == x509_cert.get_serial_number()
        )
        assert datetime.strptime(
            websocket_client_cert["notBefore"], "%b %d %H:%M:%S %Y GMT"
        ) == datetime.strptime(
            x509_cert.get_notBefore().decode("utf-8"), "%Y%m%d%H%M%SZ"
        )

    @pytest.mark.csms_tls(verify_client_certificate=True)
    @pytest.mark.ocpp_config_adaptions(
        _OCPP201NetworkConnectionProfileAdjustment(None, None, 3)
    )
    async def test_A02_use_newest_certificate_after_installation(
        self, central_system: CentralSystem, charge_point: ChargePoint201
    ):
        """Test station uses new certificate after installation

        Tests requirement A02.FR.08
        """

        # Check originally used certificate
        assert len(central_system.ws_server.websockets) == 1
        old_connection = next(iter(central_system.ws_server.websockets))
        original_certificate = old_connection.transport.get_extra_info("peercert")
        expected_original_certificate = (
            Path(__file__).parent / "../everest-aux/certs/client/csms/CSMS_RSA.pem"
        ).read_text()
        self.assert_websocket_client_sslproto_certificate_equals_certificate(
            original_certificate, expected_original_certificate
        )

        # Install new certificate by CSMS request
        csms_mock = central_system.mock
        self._setup_csms_mock(csms_mock, _CertificateSigningTestData())

        trigger_result: call_result.TriggerMessagePayload = (
            await charge_point.trigger_message_req(
                requested_message=f"SignChargingStationCertificate"
            )
        )
        await self._wait_for_mock_called(csms_mock.on_sign_certificate)

        received_csr_data = self._parse_certificate_request(
            csms_mock.on_sign_certificate.mock_calls[0].kwargs["csr"]
        )
        signed_certificate = CertificateHelper.sign_certificate_request(
            received_csr_data.csr,
            issuer_certificate_path=Path(__file__).parent
            / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.pem",
            issuer_private_key_path=Path(__file__).parent
            / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.key",
            issuer_private_key_passphrase="123456",
        )
        signed_result: call_result.CertificateSignedPayload = (
            await charge_point.certificate_signed_req(
                certificate_chain=signed_certificate,
                certificate_type="ChargingStationCertificate",
            )
        )
        assert signed_result == call_result.CertificateSignedPayload(status="Accepted")

        # Verify: wait for new connection to be established; validate certificate
        async def wait_for_reconnect():
            while len(
                central_system.ws_server.websockets
            ) < 1 or central_system.ws_server.websockets == {old_connection}:
                await asyncio.sleep(0.1)

        await asyncio.wait_for(wait_for_reconnect(), 4)
        assert len(central_system.ws_server.websockets) == 1
        new_connection = next(iter(central_system.ws_server.websockets))
        new_certificate = new_connection.transport.get_extra_info("peercert")
        self.assert_websocket_client_sslproto_certificate_equals_certificate(
            new_certificate, signed_certificate
        )

    @pytest.mark.ocpp_config_adaptions(
        _OCPP201NetworkConnectionProfileAdjustment(None, None, 3)
    )
    @pytest.mark.csms_tls(verify_client_certificate=True)
    async def test_A02_use_newest_certificate_according_to_validity(
        self,
        everest_core: EverestCore,
        test_controller: TestController,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
    ):
        """Verifies condition A02.FR.09: The Charging Station SHALL use the newest certificate, as measured by the start of the validity period."""

        assert len(central_system.ws_server.websockets) == 1
        old_connection = next(iter(central_system.ws_server.websockets))

        # Setup: install new certificates
        cert_directory = Path(
            everest_core.everest_config["active_modules"]["evse_security"][
                "config_module"
            ]["csms_leaf_cert_directory"]
        )
        key_directory = Path(
            everest_core.everest_config["active_modules"]["evse_security"][
                "config_module"
            ]["csms_leaf_key_directory"]
        )

        ca_certificate = (
            Path(__file__).parent / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.pem"
        )
        ca_key = Path(__file__).parent / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.key"
        ca_passphrase = "123456"

        # Install 3 certificates; the second is newest w.r.t. validity (shortest relative shift from now to the past)
        certificates = {}
        for cert_index, (cert_name, relative_valid_time) in enumerate(
            [("Cert1-notNewest", -50), ("Cert2-newest", -10), ("Cert3-notNewest", -100)]
        ):
            cert_req, cert_key = CertificateHelper.generate_certificate_request(
                cert_name
            )
            cert = CertificateHelper.sign_certificate_request(
                cert_req,
                issuer_certificate_path=ca_certificate,
                issuer_private_key_path=ca_key,
                issuer_private_key_passphrase=ca_passphrase,
                serial=42 + cert_index,
                relative_valid_time=relative_valid_time,
            )
            (cert_directory / f"{cert_name}.pem").write_text(cert)
            (key_directory / f"{cert_name}.key").write_text(cert_key)
            certificates[cert_name] = cert
        test_controller.stop()
        test_controller.start()

        async def wait_for_reconnect():
            while len(
                central_system.ws_server.websockets
            ) < 1 or central_system.ws_server.websockets == {old_connection}:
                await asyncio.sleep(0.1)

        await asyncio.wait_for(wait_for_reconnect(), 5)
        assert len(central_system.ws_server.websockets) == 1

        new_connection = next(iter(central_system.ws_server.websockets))
        new_certificate = new_connection.transport.get_extra_info("peercert")
        self.assert_websocket_client_sslproto_certificate_equals_certificate(
            new_certificate, certificates["Cert2-newest"]
        )

    @pytest.mark.parametrize("certificate_type", ["ChargingStation", "V2G"])
    @pytest.mark.ocpp_config_adaptions(
        GenericOCPP201ConfigAdjustment(
            [
                (
                    OCPP201ConfigVariableIdentifier(
                        "InternalCtrlr",
                        "V2GCertificateExpireCheckInitialDelaySeconds",
                        "Actual",
                    ),
                    0,
                )
            ]
        )
    )
    @pytest.mark.ocpp_config_adaptions(
        GenericOCPP201ConfigAdjustment(
            [
                (
                    OCPP201ConfigVariableIdentifier(
                        "InternalCtrlr",
                        "ClientCertificateExpireCheckInitialDelaySeconds",
                        "Actual",
                    ),
                    0,
                )
            ]
        )
    )
    @pytest.mark.ocpp_config_adaptions(
        _OCPP201NetworkConnectionProfileAdjustment(None, None, 3)
    )
    async def test_A03_install_new_if_expired(
        self,
        certificate_type,
        everest_core: EverestCore,
        test_controller: TestController,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
    ):
        """Verifies condition A03.FR.02: Expiring certificates shall be renewed."""

        csms_mock = central_system.mock
        self._setup_csms_mock(csms_mock, _CertificateSigningTestData())

        # Setup: install new certificates that shortly expire
        if certificate_type == "ChargingStation":
            cert_directory = Path(
                everest_core.everest_config["active_modules"]["evse_security"][
                    "config_module"
                ]["csms_leaf_cert_directory"]
            )
            key_directory = Path(
                everest_core.everest_config["active_modules"]["evse_security"][
                    "config_module"
                ]["csms_leaf_key_directory"]
            )

            ca_certificate = (
                Path(__file__).parent / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.pem"
            )
            ca_key = (
                Path(__file__).parent / "../everest-aux/certs/ca/csms/CSMS_ROOT_CA.key"
            )
            ca_passphrase = "123456"
        else:
            cert_directory = Path(
                everest_core.everest_config["active_modules"]["evse_security"][
                    "config_module"
                ]["secc_leaf_cert_directory"]
            )
            key_directory = Path(
                everest_core.everest_config["active_modules"]["evse_security"][
                    "config_module"
                ]["secc_leaf_cert_directory"]
            )

            ca_certificate = (
                Path(__file__).parent / "../everest-aux/certs/ca/cso/CPO_SUB_CA2.pem"
            )
            ca_key = (
                Path(__file__).parent
                / "../everest-aux/certs/client/csms/CPO_SUB_CA2.key"
            )
            ca_passphrase = "123456"

        # Remove old certificates

        for f in cert_directory.glob("*.pem"):
            f.unlink()
        for f in cert_directory.glob("*.key"):
            f.unlink()

        # Install new one almost expired

        cert_req, cert_key = CertificateHelper.generate_certificate_request(
            "almost expired"
        )
        cert = CertificateHelper.sign_certificate_request(
            cert_req,
            issuer_certificate_path=ca_certificate,
            issuer_private_key_path=ca_key,
            issuer_private_key_passphrase=ca_passphrase,
            relative_expiration_time=300,  # expires in 5 minutes
        )
        (cert_directory / "almost_expired.pem").write_text(cert)
        (key_directory / "almost_expired.key").write_text(cert_key)

        test_controller.stop()
        test_controller.start()

        await self._wait_for_mock_called(csms_mock.on_sign_certificate, timeout=10)

    async def test_A01(
        self, central_system: CentralSystem, charge_point_v201: ChargePoint201
    ):
        # Disable AuthCacheCtrlr
        r: call_result.SetVariablesPayload = (
            await charge_point_v201.set_config_variables_req(
                "SecurityCtrlr", "BasicAuthPassword", "BEEFDEADBEEFDEAD"
            )
        )
        set_variable_result: SetVariableResultType = SetVariableResultType(
            **r.set_variable_result[0]
        )
        assert set_variable_result.attribute_status == SetVariableStatusType.accepted

        # wait for reconnect
        await central_system.wait_for_chargepoint(wait_for_bootnotification=False)
