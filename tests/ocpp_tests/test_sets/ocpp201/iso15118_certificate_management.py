# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import copy
import logging
import pytest
from dataclasses import field, dataclass
from typing import List, Dict
from unittest.mock import Mock, call as mock_call

from everest.testing.ocpp_utils.central_system import CentralSystem

from ocpp.v201.enums import GetInstalledCertificateStatusType, GetCertificateIdUseType

from ocpp.v201 import call as call201

from everest.testing.core_utils._configuration.libocpp_configuration_helper import (
    GenericOCPP201ConfigAdjustment,
)

from test_sets.everest_test_utils import *  # Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from everest.testing.ocpp_utils.charge_point_utils import (
    wait_for_and_validate,
    TestUtility,
)
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.core_utils.probe_module import ProbeModule

log = logging.getLogger("iso15118CertificateManagementTest")


async def await_mock_called(mock):
    while not mock.call_count:
        await asyncio.sleep(0.1)


@pytest.fixture()
def example_certificate():
    certificate = """-----BEGIN CERTIFICATE-----
MIIFlzCCA3+gAwIBAgIUVMBWzWyLetKgv4+kDH19eo/GM6MwDQYJKoZIhvcNAQEL
BQAwWjELMAkGA1UEBhMCREUxEDAOBgNVBAgMB0dlcm1hbnkxDzANBgNVBAoMBlBp
b25peDEMMAoGA1UECwwDREVWMRowGAYDVQQDDBFUZXN0IENTTVMgUm9vdCBDQTAg
Fw0yMzEwMjMxMTQzNDJaGA80NzYxMDkxODExNDM0MlowWjELMAkGA1UEBhMCREUx
EDAOBgNVBAgMB0dlcm1hbnkxDzANBgNVBAoMBlBpb25peDEMMAoGA1UECwwDREVW
MRowGAYDVQQDDBFUZXN0IENTTVMgUm9vdCBDQTCCAiIwDQYJKoZIhvcNAQEBBQAD
ggIPADCCAgoCggIBAK0kxp3gaNU4RfhwQVA2/fGV8s1O0j6NWVOmJjidGnsghbTO
mXe8gIbCdTraMFejpofBt9X5UFm5FDRAeVF3QhgRQ4m5AzecwdWI737Lst3+FL++
ydx0I5rBrwM53p/mYKiX+bRTv0MjGmRrB2+HjUwvwNjanvdk/RTsclEXwFPo4LQd
NqOdrBGcL7KYAh+OtJLbRc9dxy18KA0KnbanrPdNh6wdRRPd4G3KdkNvLXT2PNy1
KxZgcIXHhc5jSrcBpTV/yWXWk96Sdy/yQprwF0GfMKJcEe4J6lea4l8gpiGhOGp4
s6CI0KucfTMd8qfTuIP+Rh62wkIP8psPhthJq6r/xA7wqHJ+Ae1w0qJWD6cTCzM/
l5eoPE8zI4vK04S2T9AR8o7CjPrhGQMa7z1+tn+uBoh5qIz27NJz5xCpTOA94l80
NHRlEJprEydk9YrecGi5SSBLf31OBLBycptLc2uXj4sPqzHFC0z1YG+5Nd8tHDIN
qcBepE+KgFwc0KKwgm1gtl2/s5SVBNSdM6h3dbol18r+B+29Up4F88o/DXH1OO9Y
eZuWGCltn6lhSMH+pmXTskI78o2RDFMAyndaGN0YpV9AZvkIdG2ps1Fs/VdD59P5
8fw4xh9lW2NDqK5uY+tY49aCYcA9hgiNXyEZcdZVY9que41cGeTobH1YA+rPAgMB
AAGjUzBRMB0GA1UdDgQWBBToGGVj0VQC/ZXxcdFaIgXqSp6xeDAfBgNVHSMEGDAW
gBToGGVj0VQC/ZXxcdFaIgXqSp6xeDAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3
DQEBCwUAA4ICAQCn7ELupRtTEuLIzC5fb9+bzhURBUosNsauqPIkcEZ8d33bQZGU
E5xKTlRcsKoQJ9+TK6THWZ6cXBW90lhe6db6rR0/tYLJzhGBryAjaX071Mphalzx
9gsQ3flKEwDAnpcfyRY7AZMapEzFwuXoBY1qN1gMLVCQQUGgVBbKQ8vctfZJ7EXI
Uj45ZCSp8nfoiKILFhVs3jSHknxckscFgLkb9P/nOdp24kEx9vMfj5tXuituLHFt
eIuv83dULl5tZSBS5LxQyXjaJNWJIH+Bm9zxxZQqmrwo9JFDyUYVFz2T58mJz6Fb
kS6zMhO+P/7tV4sSSnBwF7E0uh3rYmDOcBPyUvxb44WWOQ8G1Jux+X/HI6paWvFy
CZTRl/lgUYbQRURq4w7HAnHlCVvfJzPT+sr3Ruithg+jQC7BaV2zCpKUksnK/3ln
VGZaRD6xwGGXUBAxDbjXkZvMnkGr6Iu1L6OEPF97sKSrmRMd8hn9RLKPxXDiLUzC
VD5nwEkO5Poai0b4MB9K1YNtMxc17k3EBOIGPswfp0QQPPTTy2xwP2WrFU65P1G3
Zq7pg1dChb1JX1IhdJbIlwtlkA0+ZpuFAE8q84zuoTxPyi3S0DsCjAkmYBstb6wK
CAnDdUF7Zy+eXIqWUHmXHSk4hcEiAUYx8enMUPgjE8VpcPqXzxxS+Nt2Ig==
-----END CERTIFICATE-----"""

    cert_hash_data = {
        "issuer_key_hash": "0b89ba5d6aebd520cb686d75911c3b1e236ef3a5137e298e2395e97bad049a9c",  # pkcs1,
        # "issuer_key_hash": 'a6e29e28f8f019381f712fbd19792a4247812f7faccc7ef73db78adb8ee59132',  # default
        "issuer_name_hash": "608964fb2fa9b01051979832e94b5dfc69f41dc76e94321bff1489220f0edd51",
        "serial_number": "54c056cd6c8b7ad2a0bf8fa40c7d7d7a8fc633a3",
        "hash_algorithm": "SHA256",
    }

    return {"certificate": certificate, "certificate_hash_data": cert_hash_data}


@dataclass(frozen=True)
class CertificateHashData:
    issuer_key_hash: str
    issuer_name_hash: str
    serial_number: str
    hash_algorithm: str = "SHA256"

    def __repr__(self):
        return f"CertificateHashData(Serial: {self.serial_number} Issuer: {self.issuer_key_hash[:6]}... / {self.issuer_name_hash[:6]}... )"


@dataclass(frozen=True, unsafe_hash=True)
class CertificateHashDataChainEntry:
    certificate_type: str
    certificate_hash_data: CertificateHashData
    child_certificate_hash_data: List[CertificateHashData] = field(hash=False)

    @staticmethod
    def from_dict(data: Dict):
        return CertificateHashDataChainEntry(
            certificate_type=data["certificate_type"],
            certificate_hash_data=CertificateHashData(**data["certificate_hash_data"]),
            child_certificate_hash_data=[
                CertificateHashData(**d)
                for d in data.get("child_certificate_hash_data", [])
            ],
        )

    def __repr__(self):
        s = f"( {self.certificate_type}: {self.certificate_hash_data}"
        if self.child_certificate_hash_data:
            s += "\n children: \n"
            s += "\n".join(f"\t\t{c}" for c in self.child_certificate_hash_data)
            s += "\n"
        s += ")"
        return s


@dataclass(frozen=True)
class CertificateHashDataChain:
    entries: List[CertificateHashDataChainEntry]

    @staticmethod
    def from_list(data: list[dict]):
        return CertificateHashDataChain(
            entries=[CertificateHashDataChainEntry.from_dict(d) for d in data]
        )

    def __eq__(self, other):
        return set(self.entries) == set(other.entries)

    def __repr__(self):
        return (
            "CertificateHashDataChain(\n"
            + "\n".join(f"\t{d}" for d in self.entries)
            + "\n)"
        )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config("everest-config-ocpp201-probe-module.yaml")
@pytest.mark.inject_csms_mock
@pytest.mark.probe_module
class TestIso15118CertificateManagementOcppIntegration:
    """ """

    # ************************************************************************************************
    # Use Case M1-M2: EV sends install or update Certificate request
    # ************************************************************************************************

    @pytest.mark.parametrize(
        "skip_implementation",
        [
            {
                "ProbeModuleConnectorA": ["set_get_certificate_response"],
                "ProbeModuleConnectorB": ["set_get_certificate_response"],
            }
        ],
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
    @pytest.mark.parametrize(
        "response_status", ["Accepted", "Failed"], ids=["successful", "failed"]
    )
    @pytest.mark.parametrize(
        "action",
        ["Install", "Update"],
        ids=["M01 - Certificate installation", "M02 - Certificate Update EV"],
    )
    async def test_m1_m2_certificate_request_ev(
        self,
        action,
        response_status,
        central_system: CentralSystem,
        probe_module,
        test_utility: TestUtility,
    ):
        """
        Tests Error handling of M01 and M02
        Tested requirements: M01.FR.01, MR02.FR.01
        """
        connectors = ["ProbeModuleConnectorA", "ProbeModuleConnectorB"]

        mock_cmd_set_get_certificate_response = {}
        for connector_id in connectors:
            mock_cmd_set_get_certificate_response[connector_id] = Mock()
            mock_cmd_set_get_certificate_response[connector_id].return_value = None
            probe_module.implement_command(
                connector_id,
                "set_get_certificate_response",
                mock_cmd_set_get_certificate_response[connector_id],
            )

        # start and ready probe module EvseManagers and wait for libocpp to connect
        probe_module.start()
        await probe_module.wait_to_be_ready()
        probe_module.publish_variable("ProbeModuleConnectorA", "ready", True)
        probe_module.publish_variable("ProbeModuleConnectorB", "ready", True)
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Each connector sends an installation request
        for connector_index, calling_connector_id in enumerate(connectors):

            # Setup ChargePoint response

            exi_response = f"mock exi response for {calling_connector_id}"

            central_system.mock.on_get_15118_ev_certificate.side_effect = [
                call_result201.Get15118EVCertificatePayload(
                    status=response_status, exi_response=exi_response
                )
            ]

            # Act: Publish Install Certificate requeset
            mock_certificate_installation_req = base64.b64encode(
                f"{calling_connector_id} mock Raw CertificateInstallationReq or CertificateUpdateReq message as exi stream".encode(
                    "utf-8"
                )
            ).decode("utf-8")

            mock_iso15118_schema_version = f"{calling_connector_id} mock Schema Version"

            probe_module.publish_variable(
                calling_connector_id,
                "iso15118_certificate_request",
                {
                    "exi_request": mock_certificate_installation_req,
                    "iso15118_schema_version": mock_iso15118_schema_version,
                    "certificate_action": action,
                },
            )

            # Verify: CSMS is called correctly
            expected_cp_request = call201.Get15118EVCertificatePayload(
                iso15118_schema_version=mock_iso15118_schema_version,
                exi_request=mock_certificate_installation_req,
                action=action,
            )

            assert await wait_for_and_validate(
                test_utility,
                chargepoint_with_pm,
                exp_action="Get15118EVCertificate",
                exp_payload=expected_cp_request,
            )

            # Verify: Certificate response forwarded to correct EVSE manager as commmand
            called_mock = mock_cmd_set_get_certificate_response[calling_connector_id]
            other_connector_id = connectors[(connector_index + 1) % len(connectors)]
            uncalled_mock = mock_cmd_set_get_certificate_response[other_connector_id]

            await asyncio.wait_for(await_mock_called(called_mock), 3)

            assert called_mock.mock_calls == [
                mock_call(
                    {
                        "certificate_response": {
                            "certificate_action": action,
                            "exi_response": exi_response,
                            "status": response_status,
                        }
                    }
                )
            ]
            assert uncalled_mock.mock_calls == []

            for mock in mock_cmd_set_get_certificate_response.values():
                mock.reset_mock()

    # ************************************************************************************************
    # Use Case M3:  Install CA certificate in a Charging Station
    # ************************************************************************************************

    @pytest.mark.parametrize(
        "skip_implementation", [{"ProbeModuleSecurity": ["get_installed_certificates"]}]
    )
    @pytest.mark.asyncio
    async def test_m3_get_installed_certificates(
        self, central_system: CentralSystem, probe_module: ProbeModule
    ):
        """
        Integration test for use case M03 - Retrieve list of available certificates from a Charging Station
        The EvseSecurity module is mocked up by the ProbeModule here.

        Tests requirements M03.FR.01, M03.FR.03, M03.FR.04, M03.FR.05 (by checking response from security module is forwarded)
        """

        # Data that is returned by the mocked EvSecurity Module: dict[str, CertificateHashDataChain]
        # see type definitions evse_security
        mock_certificate_hash_data_chain_data = {
            "CSMSRootCertificate": {
                "certificate_type": "CSMSRootCertificate",
                "certificate_hash_data": {
                    "hash_algorithm": "SHA256",
                    "issuer_key_hash": "mock key_hash<CSMSRootCertificate>",
                    "issuer_name_hash": "mock issuer_name_key_hash<CSMSRootCertificate>",
                    "serial_number": "1",
                },
            },
            "V2GCertificateChain": {
                "certificate_type": "V2GCertificateChain",
                "certificate_hash_data": {
                    "hash_algorithm": "SHA256",
                    "issuer_key_hash": "mock key_hash<V2GCertificateChain>",
                    "issuer_name_hash": "mock issuer_name_key_hash<V2GCertificateChain>",
                    "serial_number": "2",
                },
                "child_certificate_hash_data": [
                    {
                        "hash_algorithm": "SHA256",
                        "issuer_key_hash": "mock key_hash<V2GCertificateChainChild0>",
                        "issuer_name_hash": "mock issuer_name_key_hash<V2GCertificateChainChild0>",
                        "serial_number": "3",
                    },
                    {
                        "hash_algorithm": "SHA256",
                        "issuer_key_hash": "mock key_hash<V2GCertificateChainChild1>",
                        "issuer_name_hash": "mock issuer_name_key_hash<V2GCertificateChainChild1>",
                        "serial_number": "4",
                    },
                ],
            },
        }

        # Setup:  Probe module mimics security module's get_installed_certificates command
        def security_module_get_certs_mock(args):
            return {
                "status": "Accepted",
                "certificate_hash_data_chain": [
                    copy.deepcopy(
                        mock_certificate_hash_data_chain_data[certificate_type]
                    )
                    for certificate_type in args["certificate_types"]
                ],
            }

        security_module_mock = Mock()
        security_module_mock.side_effect = security_module_get_certs_mock
        probe_module.implement_command(
            "ProbeModuleSecurity", "get_installed_certificates", security_module_mock
        )

        # start and ready probe module EvseManagers and wait for libocpp to connect
        probe_module.start()
        await probe_module.wait_to_be_ready()
        probe_module.publish_variable("ProbeModuleConnectorA", "ready", True)
        probe_module.publish_variable("ProbeModuleConnectorB", "ready", True)
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Act: request certs
        ocpplib_result: call_result201.GetInstalledCertificateIdsPayload = (
            await chargepoint_with_pm.get_installed_certificate_ids_req(
                certificate_type=[
                    GetCertificateIdUseType.csms_root_certificate,
                    GetCertificateIdUseType.v2g_certificate_chain,
                ]
            )
        )

        # Verfiy
        assert ocpplib_result == call_result201.GetInstalledCertificateIdsPayload(
            status="Accepted",
            certificate_hash_data_chain=[
                mock_certificate_hash_data_chain_data["CSMSRootCertificate"],
                mock_certificate_hash_data_chain_data["V2GCertificateChain"],
            ],
        )
        assert security_module_mock.mock_calls == [
            mock_call(
                {"certificate_types": ["CSMSRootCertificate", "V2GCertificateChain"]}
            )
        ]

    @pytest.mark.parametrize(
        "skip_implementation", [{"ProbeModuleSecurity": ["get_installed_certificates"]}]
    )
    async def test_m3_get_installed_certificates_not_found(
        self, central_system: CentralSystem, probe_module: ProbeModule
    ):
        """
        Integration test for use case M03 - Retrieve list of available certificates from a Charging Station, but certificate is not found
        The EvseSecurity module is mocked up by the ProbeModule here.

        Tests requirements M03.FR.01, M03.FR.02
        """

        # Data that is returned by the mocked EvSecurity Module: dict[str, CertificateHashDataChain]
        # see type definitions evse_security

        # Setup:  Probe module mimics security module's get_installed_certificates command
        def security_module_get_certs_mock(args):
            return {"status": "NotFound", "certificate_hash_data_chain": []}

        security_module_mock = Mock()
        security_module_mock.side_effect = security_module_get_certs_mock
        probe_module.implement_command(
            "ProbeModuleSecurity", "get_installed_certificates", security_module_mock
        )

        # start and ready probe module EvseManagers and wait for libocpp to connect
        probe_module.start()
        await probe_module.wait_to_be_ready()
        probe_module.publish_variable("ProbeModuleConnectorA", "ready", True)
        probe_module.publish_variable("ProbeModuleConnectorB", "ready", True)
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Act: request certs
        ocpplib_result: call_result201.GetInstalledCertificateIdsPayload = (
            await chargepoint_with_pm.get_installed_certificate_ids_req(
                certificate_type=[
                    GetCertificateIdUseType.csms_root_certificate,
                    GetCertificateIdUseType.v2g_certificate_chain,
                ]
            )
        )

        # Verfiy
        assert ocpplib_result == call_result201.GetInstalledCertificateIdsPayload(
            status="NotFound", certificate_hash_data_chain=None
        )
        assert security_module_mock.mock_calls == [
            mock_call(
                {"certificate_types": ["CSMSRootCertificate", "V2GCertificateChain"]}
            )
        ]

    # ************************************************************************************************
    # Use Case M04 - Delete a specific certificate from a Charging Station
    # ************************************************************************************************

    @pytest.mark.parametrize(
        "skip_implementation", [{"ProbeModuleSecurity": ["delete_certificate"]}]
    )
    @pytest.mark.parametrize("response_status", ["Accepted", "Failed", "NotFound"])
    async def test_m4_delete(
        self,
        response_status,
        central_system: CentralSystem,
        probe_module: ProbeModule,
        test_utility: TestUtility,
    ):
        """
        Integration test for use case M04 - Delete a specific certificate from a Charging Station
        The EvseSecurity module is mocked up by the ProbeModule here.

        Tests requirements M04.FR.01, M03.FR.02, M03.FR.03, M03.FR.04;  only implicitly  M03.FR.06, M03.FR.07 M03.FR.08 (this is forwarded to the
        mocked security module)
        """

        cert_hash_data: dict[str, str] = {
            "hash_algorithm": "SHA256",
            "issuer_key_hash": "89ea6977e786fcbaeb4f04e4ccdbfaa6a6088e8ba8f7404033ac1b3a62bc36a1",
            "issuer_name_hash": "e60bd843bf2279339127ca19ab6967081dd6f95e745dc8b8632fa56031debe5b",
            "serial_number": "1",
        }

        # setup probe module additional functions

        security_module_mock = Mock()
        security_module_mock.side_effect = [response_status]

        probe_module.implement_command(
            "ProbeModuleSecurity", "delete_certificate", security_module_mock
        )

        # start and ready probe module EvseManagers and wait for libocpp to connect
        probe_module.start()
        await probe_module.wait_to_be_ready()
        probe_module.publish_variable("ProbeModuleConnectorA", "ready", True)
        probe_module.publish_variable("ProbeModuleConnectorB", "ready", True)
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        await chargepoint_with_pm.delete_certificate_req(
            certificate_hash_data=copy.deepcopy(cert_hash_data)
        )
        assert await wait_for_and_validate(
            test_utility,
            chargepoint_with_pm,
            "DeleteCertificate",
            call_result201.DeleteCertificatePayload(response_status),
        )
        assert security_module_mock.mock_calls == [
            mock_call({"certificate_hash_data": cert_hash_data})
        ]

    # ************************************************************************************************
    # Use Case M5: M05 - Install CA certificate in a Charging Station
    # ************************************************************************************************

    @pytest.mark.parametrize(
        "skip_implementation", [{"ProbeModuleSecurity": ["install_ca_certificate"]}]
    )
    @pytest.mark.parametrize(
        "evse_security_response_status, chargepoint_response_status",
        [
            ("Accepted", "Accepted"),
            ("WriteError", "Failed"),
            ("InvalidFormat", "Rejected"),
        ],
    )
    async def test_m5_install(
        self,
        evse_security_response_status,
        chargepoint_response_status,
        test_config: OcppTestConfiguration,
        central_system: CentralSystem,
        probe_module: ProbeModule,
        test_utility: TestUtility,
    ):
        """
        Integration test for use case M05 - Install CA certificate in a Charging Station
        The EvseSecurity module is mocked up by the ProbeModule here.

        Tested requirements: M05.FR.01, M05.FR.02, M05.FR.03, M05.FR.06, M05.FR.07; remaining only implicit
        """

        request = {
            "certificate_type": "CSMSRootCertificate",
            "certificate": "mock certificate",
        }

        security_module_mock = Mock()
        security_module_mock.side_effect = [evse_security_response_status]
        probe_module.implement_command(
            "ProbeModuleSecurity", "install_ca_certificate", security_module_mock
        )

        # start and ready probe module EvseManagers and wait for libocpp to connect
        probe_module.start()
        await probe_module.wait_to_be_ready()
        probe_module.publish_variable("ProbeModuleConnectorA", "ready", True)
        probe_module.publish_variable("ProbeModuleConnectorB", "ready", True)
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        await chargepoint_with_pm.install_certificate_req(**request)
        assert await wait_for_and_validate(
            test_utility,
            chargepoint_with_pm,
            "InstallCertificate",
            call_result201.InstallCertificatePayload(chargepoint_response_status),
        )

        assert security_module_mock.mock_calls == [
            mock_call(
                {"certificate": request["certificate"], "certificate_type": "CSMS"}
            )
        ]


# ************************************************************************************************
# E2E Tests
# ************************************************************************************************


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config("everest-config-ocpp201.yaml")
@pytest.mark.source_certs_dir(Path(__file__).parent.parent / "everest-aux/certs")
@pytest.mark.use_temporary_persistent_store
class TestIso15118CertificateManagementE2E:
    """
    E2E Tests between the mocked CSMS and the SIL Everest stack for Iso15118 Certificate Management.
    """

    @pytest.mark.parametrize(
        "certificate_type, use_type, certificate_file",
        [
            (
                "CSMSRootCertificate",
                GetCertificateIdUseType.csms_root_certificate,
                "csms/CSMS_ROOT_CA.pem",
            ),
            (
                "ManufacturerRootCertificate",
                GetCertificateIdUseType.manufacturer_root_certificate,
                "mf/MF_ROOT_CA.pem",
            ),
            (
                "V2GRootCertificate",
                GetCertificateIdUseType.v2g_root_certificate,
                "v2g/V2G_ROOT_CA.pem",
            ),
            (
                "MORootCertificate",
                GetCertificateIdUseType.mo_root_certificate,
                "mo/MO_ROOT_CA.pem",
            ),
        ],
    )
    async def test_m3_retrieve_installed_certificates(
        self,
        certificate_type,
        use_type,
        certificate_file,
        test_config: OcppTestConfiguration,
        charge_point_v201: ChargePoint201,
    ):
        search_path = test_config.certificate_info.csms_root_ca.parent.parent

        result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req(
                certificate_type=[use_type]
            )
        )

        assert result == call_result201.GetInstalledCertificateIdsPayload(
            status=GetInstalledCertificateStatusType.accepted,
            certificate_hash_data_chain=[
                {
                    "certificate_hash_data": CertificateHashDataGenerator.get_hash_data(
                        certificate_path=search_path / certificate_file
                    ),
                    "certificate_type": certificate_type,
                }
            ],
        )

    async def test_m3_retrieve_installed_certificates_all_types(
        self, test_config: OcppTestConfiguration, charge_point_v201: ChargePoint201
    ):
        """
        Mimics OCTT M_ISO_15118_CertificateManagement_CS - TC_M_18_CS-Retrieve certificates from Charging Station
        """
        search_path = test_config.certificate_info.csms_root_ca.parent.parent

        result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req()
        )
        assert result.status == "Accepted"

        expected_certificate_hash_data_chain_set = {
            CertificateHashDataChainEntry.from_dict(
                {
                    "certificate_hash_data": CertificateHashDataGenerator.get_hash_data(
                        certificate_path=search_path / certificate_file
                    ),
                    "certificate_type": certificate_type,
                }
            )
            for certificate_type, use_type, certificate_file in [
                (
                    "CSMSRootCertificate",
                    GetCertificateIdUseType.csms_root_certificate,
                    "csms/CSMS_ROOT_CA.pem",
                ),
                (
                    "ManufacturerRootCertificate",
                    GetCertificateIdUseType.manufacturer_root_certificate,
                    "mf/MF_ROOT_CA.pem",
                ),
                (
                    "MORootCertificate",
                    GetCertificateIdUseType.mo_root_certificate,
                    "mo/MO_ROOT_CA.pem",
                ),
                (
                    "V2GRootCertificate",
                    GetCertificateIdUseType.v2g_root_certificate,
                    "v2g/V2G_ROOT_CA.pem",
                ),
            ]
        } | {
            CertificateHashDataChainEntry.from_dict(d)
            for d in self._get_v2g_certificate_chain(search_path)
        }

        assert {
            CertificateHashDataChainEntry.from_dict(d)
            for d in result.certificate_hash_data_chain
        } == expected_certificate_hash_data_chain_set

    async def test_m3_retrieve_installed_certificates_not_found(
        self,
        tmp_path,
        # todo: replace by evse security config fixture
        charge_point_v201: ChargePoint201,
    ):
        """
        Mimics OCTT M_ISO_15118_CertificateManagement_CS - TC_M_19_CS-Retrieve certificates from Charging Station
        """

        # Prerequisite of  TC_M_19_CS-Retrieve : "The Charging Station does not have a MORootCertificate installed."

        for f in list((tmp_path / "certs/ca/mo").glob("*.pem")) + list(
            (tmp_path / "certs/ca/mo").glob("*.der")
        ):
            f.unlink()

        result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req(
                certificate_type=[GetCertificateIdUseType.mo_root_certificate]
            )
        )
        assert result == call_result201.GetInstalledCertificateIdsPayload(
            status="NotFound"
        )

    def _get_v2g_certificate_chain(self, ca_cert_path: Path):
        root_ca_path = ca_cert_path / "v2g/V2G_ROOT_CA.pem"
        sub_ca_1_path = ca_cert_path / "cso/CPO_SUB_CA1.pem"
        sub_ca_2_path = ca_cert_path / "cso/CPO_SUB_CA2.pem"
        leaf_cert_path = ca_cert_path.parent / "client/cso/SECC_LEAF.pem"

        exp_hashdata_ca_1 = CertificateHashDataGenerator.get_hash_data(
            certificate_path=sub_ca_1_path, issuer_certificate_path=root_ca_path
        )
        exp_hashdata_ca_2 = CertificateHashDataGenerator.get_hash_data(
            certificate_path=sub_ca_2_path, issuer_certificate_path=sub_ca_1_path
        )
        exp_hashdata_leaf = CertificateHashDataGenerator.get_hash_data(
            certificate_path=leaf_cert_path, issuer_certificate_path=sub_ca_2_path
        )

        return [
            {
                "certificate_type": "V2GCertificateChain",
                "certificate_hash_data": exp_hashdata_leaf,
                "child_certificate_hash_data": [exp_hashdata_ca_1, exp_hashdata_ca_2],
            }
        ]

    async def test_m3_retrieve_v2g_certificate_chain(
        self, test_config: OcppTestConfiguration, charge_point_v201: ChargePoint201
    ):
        """
        Quoting the OCPP 2.0.1. spec, req. M03.FR.05:
        The Charging Station SHALL include the hash data for each
        installed certificate belonging to a V2G certificate chain. Sub CA
        certificates SHALL be placed as a childCertificate under the V2G
        Charging Station certificate.

        This means that we expect one entry for each v2g leaf cert, with the sub-CAs added as child certificates
        The v2g root should not be included in the chain.
        The leaf cert is available at certs/client/cso/SECC_LEAF.pem
        the sub-CA certs are available at certs/ca/cso/CPO_SUB_CA{1,2}.pem
        """

        # Prepare: Expected hash data
        use_type = GetCertificateIdUseType.v2g_certificate_chain

        # Act
        result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req(
                certificate_type=[use_type]
            )
        )

        # Verify
        assert result.status == GetInstalledCertificateStatusType.accepted

        resulting_chain = CertificateHashDataChain.from_list(
            result.certificate_hash_data_chain
        )
        expected_chain = CertificateHashDataChain.from_list(
            self._get_v2g_certificate_chain(
                test_config.certificate_info.csms_root_ca.parent.parent
            )
        )
        assert resulting_chain == expected_chain

    @pytest.mark.parametrize(
        "certificate_type, use_type, certificate_file",
        [
            (
                "CSMSRootCertificate",
                GetCertificateIdUseType.csms_root_certificate,
                "csms/CSMS_ROOT_CA.pem",
            ),
            (
                "ManufacturerRootCertificate",
                GetCertificateIdUseType.manufacturer_root_certificate,
                "mf/MF_ROOT_CA.pem",
            ),
            (
                "V2GRootCertificate",
                GetCertificateIdUseType.v2g_root_certificate,
                "v2g/V2G_ROOT_CA.pem",
            ),
            (
                "MORootCertificate",
                GetCertificateIdUseType.mo_root_certificate,
                "mo/MO_ROOT_CA.pem",
            ),
        ],
    )
    async def test_m4_delete_and_retrieve_certificates(
        self,
        certificate_type,
        use_type,
        certificate_file,
        test_config,
        charge_point_v201: ChargePoint201,
    ):
        certificate_search_path = (
            test_config.certificate_info.csms_root_ca.parent.parent
        )

        certificate_for_deletion_hash_data = CertificateHashDataGenerator.get_hash_data(
            certificate_path=certificate_search_path / certificate_file
        )

        deletion_result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.delete_certificate_req(
                certificate_hash_data=certificate_for_deletion_hash_data
            )
        )

        assert deletion_result.status == GetInstalledCertificateStatusType.accepted

        verification_result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req(
                certificate_type=[use_type]
            )
        )

        assert verification_result.status == GetInstalledCertificateStatusType.notFound

    async def test_m4_reject_deletion_of_charging_station_certificate(
        self, test_config, charge_point_v201
    ):
        """
        M_ISO_15118_CertificateManagement_CS - TC_M_23_CS-Delete a certificate from a Charging Station
        """

        cso_certificate = (
            test_config.certificate_info.csms_root_ca.parent.parent.parent
            / "client"
            / "csms"
            / "CSMS_RSA.pem"
        )
        issuer_certificate = (
            test_config.certificate_info.csms_root_ca.parent.parent.parent
            / "ca"
            / "csms"
            / "CSMS_ROOT_CA.pem"
        )
        certificate_for_deletion_hash_data = CertificateHashDataGenerator.get_hash_data(
            certificate_path=cso_certificate, issuer_certificate_path=issuer_certificate
        )

        deletion_result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.delete_certificate_req(
                certificate_hash_data=certificate_for_deletion_hash_data
            )
        )

        assert deletion_result.status == "Failed"

    async def test_m4_allow_deletion_of_secc_leaf_certificate(
        self, test_config, charge_point_v201
    ):
        """
        M_ISO_15118_CertificateManagement_CS - TC_M_23_CS-Delete a certificate from a Charging Station
        """

        cso_certificate = (
            test_config.certificate_info.csms_root_ca.parent.parent.parent
            / "client"
            / "cso"
            / "SECC_LEAF.pem"
        )
        issuer_certificate = (
            test_config.certificate_info.csms_root_ca.parent.parent.parent
            / "ca"
            / "cso"
            / "CPO_SUB_CA2.pem"
        )
        certificate_for_deletion_hash_data = CertificateHashDataGenerator.get_hash_data(
            certificate_path=cso_certificate, issuer_certificate_path=issuer_certificate
        )

        deletion_result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.delete_certificate_req(
                certificate_hash_data=certificate_for_deletion_hash_data
            )
        )

        assert deletion_result.status == "Accepted"

    @pytest.mark.parametrize(
        "certificate_type, ocpp_certificate_type",
        [
            ("CSMSRootCertificate", GetCertificateIdUseType.csms_root_certificate),
            (
                "ManufacturerRootCertificate",
                GetCertificateIdUseType.manufacturer_root_certificate,
            ),
            ("V2GRootCertificate", GetCertificateIdUseType.v2g_root_certificate),
            ("MORootCertificate", GetCertificateIdUseType.mo_root_certificate),
        ],
    )
    async def test_m5_install_ca_certificate(
        self,
        example_certificate,
        certificate_type,
        ocpp_certificate_type,
        charge_point_v201: ChargePoint201,
    ):
        certificate, cert_hash_data = (
            example_certificate["certificate"],
            example_certificate["certificate_hash_data"],
        )

        certificates_before: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req(
                certificate_type=[ocpp_certificate_type]
            )
        )

        logging.info(
            f"Installing certificate (serial: {cert_hash_data['serial_number']}) as {ocpp_certificate_type}"
        )
        res: call_result201.InstallCertificatePayload = (
            await charge_point_v201.install_certificate_req(
                certificate_type=ocpp_certificate_type, certificate=certificate
            )
        )

        assert res == call_result201.InstallCertificatePayload(status="Accepted")

        verification_result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req(
                certificate_type=[ocpp_certificate_type]
            )
        )
        assert verification_result.status == "Accepted"
        assert {
            CertificateHashDataChainEntry.from_dict(d)
            for d in verification_result.certificate_hash_data_chain
        } == {
            CertificateHashDataChainEntry.from_dict(
                {
                    "certificate_hash_data": cert_hash_data,
                    "certificate_type": certificate_type,
                }
            )
        } | {
            CertificateHashDataChainEntry.from_dict(d)
            for d in certificates_before.certificate_hash_data_chain
        }

    async def test_m5_reject_installation_of_expired_certificate(
        self, charge_point_v201
    ):
        """Mimics  M_ISO_15118_CertificateManagement_CS - TC_M_07_CS-Install CA certificate"""

        cert = """-----BEGIN CERTIFICATE-----
MIICvzCCAacCAhI0MA0GCSqGSIb3DQEBBAUAMCUxCzAJBgNVBAYTAkRFMRYwFAYD
VQQDDA1FeHBpcmVkUm9vdENBMB4XDTAxMTIxMzAwMDAwMFoXDTAyMTIxMzAwMDAw
MFowJTELMAkGA1UEBhMCREUxFjAUBgNVBAMMDUV4cGlyZWRSb290Q0EwggEiMA0G
CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC6/YDFyLjc/NrhEGAojh3xxpIhYLWf
2xQ+8Feh/YLS3S7Jeavz4uJykbKqndbdjYK/XMmMKsh/oL6UxZERM9qxeSzIsP+b
p5U6boepS5RE71NhyLjHqLKb2fm3Zrn3UX0dHVt1R3OUSqSzU33hU84kqNd90zBJ
hTmyFokHjklpElt6NkiU94aA/A5hlXxNw5f+bGdHWz2blmzH0ZElCcX4yQ5Q9YZ4
liXz6rVw5WiLBf3xCOpATxvbWjYuqmd6JhmfVj8vwkLsKSfGmEGQyth1C91enf4I
s06hXg2hHpJq3AfYVTEpYNa/6l6H3lhGbM/jdbBLVSBSs9ceSi/jFPe3AgMBAAEw
DQYJKoZIhvcNAQEEBQADggEBAJDbF55CQYaULoOnLmXGi/RRBL9V5MyeVAsIk2/k
oKVJILGTejGD7WSFvnQF9OS2DhI0TPoxfbpRbZ7vrQ6yA9ddW0xXofnglSPwoAiR
5HMMSk/N5FTHtfArhwz6lltYexeBtYeRbCEilphGHaYPl0dIdBaFay8nm5SwHtFD
gUN7wPcaUwSfD+DnLJGYxcui8eUlpM6o+xUxQ41EdKgCpE/4hkTZz3osmtYph/yG
EZH3hVCk+BjehE9B/9CvbnLiukasDewAxjctSOPJrP2Z58+RRiXiQodeoDVxxvG4
Pjw/OEvVm/QqKQQDc2q2ZIs8RsvbpeNZD84mJT706EqID3s=
-----END CERTIFICATE-----"""

        expired_certificate = cert

        res: call_result201.InstallCertificatePayload = (
            await charge_point_v201.install_certificate_req(
                certificate_type=GetCertificateIdUseType.csms_root_certificate,
                certificate=expired_certificate,
            )
        )

        assert res == call_result201.InstallCertificatePayload(status="Rejected")

    @pytest.mark.parametrize(
        "certificate_type, ocpp_certificate_type",
        [
            ("CSMSRootCertificate", GetCertificateIdUseType.csms_root_certificate),
            (
                "ManufacturerRootCertificate",
                GetCertificateIdUseType.manufacturer_root_certificate,
            ),
            ("V2GRootCertificate", GetCertificateIdUseType.v2g_root_certificate),
            ("MORootCertificate", GetCertificateIdUseType.mo_root_certificate),
        ],
    )
    async def test_m4_delete_installed_certificates(
        self,
        certificate_type,
        ocpp_certificate_type,
        example_certificate,
        test_config,
        charge_point_v201: ChargePoint201,
    ):
        certificate, cert_hash_data = (
            example_certificate["certificate"],
            example_certificate["certificate_hash_data"],
        )

        certificates_before: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req(
                certificate_type=[ocpp_certificate_type]
            )
        )
        assert certificates_before.status == GetInstalledCertificateStatusType.accepted

        installation_result: call_result201.InstallCertificatePayload = (
            await charge_point_v201.install_certificate_req(
                certificate_type=ocpp_certificate_type, certificate=certificate
            )
        )
        assert installation_result == call_result201.InstallCertificatePayload(
            status="Accepted"
        )

        certificates_after_install: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req(
                certificate_type=[ocpp_certificate_type]
            )
        )

        assert cert_hash_data in [
            c["certificate_hash_data"]
            for c in certificates_after_install.certificate_hash_data_chain
        ]

        deletion_result: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.delete_certificate_req(
                certificate_hash_data=cert_hash_data
            )
        )

        assert deletion_result.status == GetInstalledCertificateStatusType.accepted

        certificates_after_delete: call_result201.GetInstalledCertificateIdsPayload = (
            await charge_point_v201.get_installed_certificate_ids_req(
                certificate_type=[ocpp_certificate_type]
            )
        )

        assert (
            certificates_after_delete.status
            == GetInstalledCertificateStatusType.accepted
        )
        assert (
            certificates_before.certificate_hash_data_chain
            == certificates_after_delete.certificate_hash_data_chain
        )
