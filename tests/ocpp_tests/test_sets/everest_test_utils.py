# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from __future__ import annotations

import asyncio
import hashlib
import queue
import os
from pathlib import Path
import threading
from types import FunctionType
from typing import Optional

from OpenSSL import crypto
from datetime import datetime, timedelta

from cryptography import x509
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.x509 import load_pem_x509_certificate

from everest.testing.core_utils._configuration.libocpp_configuration_helper import (
    GenericOCPP201ConfigAdjustment,
    OCPP201ConfigVariableIdentifier,
)

from iso15118.shared.security import (
    CertPath,
    KeyEncoding,
    KeyPasswordPath,
    KeyPath,
    create_signature,
    encrypt_priv_key,
    get_cert_cn,
    load_cert,
    load_priv_key,
)
from iso15118.shared.messages.iso15118_2.msgdef import V2GMessage as V2GMessageV2
from iso15118.shared.messages.iso15118_2.header import MessageHeader as MessageHeaderV2
from iso15118.shared.messages.iso15118_2.datatypes import (
    EMAID,
    CertificateChain,
    DHPublicKey,
    EncryptedPrivateKey,
    ResponseCode,
    SubCertificates,
)
from iso15118.shared.messages.iso15118_2.body import Body, CertificateInstallationRes
from iso15118.shared.messages.enums import Namespace
from iso15118.shared.exi_codec import EXI
from iso15118.shared.exificient_exi_codec import ExificientEXICodec
from iso15118.shared.exceptions import EncryptionError, PrivateKeyReadError
import json
import base64

from everest.testing.ocpp_utils.charge_point_utils import (
    OcppTestConfiguration,
    ChargePointInfo,
    CertificateInfo,
    FirmwareInfo,
    AuthorizationInfo,
)

from ocpp.charge_point import snake_to_camel_case, asdict, remove_nones
from ocpp.v16 import call, call_result
from ocpp.v16.enums import Action, DataTransferStatus
from ocpp.routing import on

# for OCPP1.6 PnC whitepaper:
from ocpp.v201 import call_result as call_result201
from ocpp.v201.datatypes import IdTokenInfoType
from ocpp.v201.enums import (
    AuthorizationStatusType,
    GenericStatusType,
    Iso15118EVCertificateStatusType,
    GetCertificateStatusType,
)


class EXIGenerator:

    def __init__(self, certs_path):
        self.certs_path = certs_path
        EXI().set_exi_codec(ExificientEXICodec())

    def generate_certificate_installation_res(
        self, base64_encoded_cert_installation_req: str, namespace: str
    ) -> str:

        cert_install_req_exi = base64.b64decode(base64_encoded_cert_installation_req)
        cert_install_req = EXI().from_exi(cert_install_req_exi, namespace)
        try:
            dh_pub_key, encrypted_priv_key_bytes = encrypt_priv_key(
                oem_prov_cert=load_cert(
                    os.path.join(self.certs_path, CertPath.OEM_LEAF_DER)
                ),
                priv_key_to_encrypt=load_priv_key(
                    os.path.join(self.certs_path, KeyPath.CONTRACT_LEAF_PEM),
                    KeyEncoding.PEM,
                    os.path.join(
                        self.certs_path, KeyPasswordPath.CONTRACT_LEAF_KEY_PASSWORD
                    ),
                ),
            )
        except EncryptionError:
            raise EncryptionError(
                "EncryptionError while trying to encrypt the private key for the "
                "contract certificate"
            )
        except PrivateKeyReadError as exc:
            raise PrivateKeyReadError(
                f"Can't read private key to encrypt for CertificateInstallationRes:"
                f" {exc}"
            )

        # The elements that need to be part of the signature
        contract_cert_chain = CertificateChain(
            id="id1",
            certificate=load_cert(
                os.path.join(self.certs_path, CertPath.CONTRACT_LEAF_DER)
            ),
            sub_certificates=SubCertificates(
                certificates=[
                    load_cert(os.path.join(self.certs_path, CertPath.MO_SUB_CA2_DER)),
                    load_cert(os.path.join(self.certs_path, CertPath.MO_SUB_CA1_DER)),
                ]
            ),
        )
        encrypted_priv_key = EncryptedPrivateKey(
            id="id2", value=encrypted_priv_key_bytes
        )
        dh_public_key = DHPublicKey(id="id3", value=dh_pub_key)
        emaid = EMAID(
            id="id4",
            value=get_cert_cn(
                load_cert(os.path.join(self.certs_path, CertPath.CONTRACT_LEAF_DER))
            ),
        )
        cps_certificate_chain = CertificateChain(
            certificate=load_cert(os.path.join(self.certs_path, CertPath.CPS_LEAF_DER)),
            sub_certificates=SubCertificates(
                certificates=[
                    load_cert(os.path.join(self.certs_path, CertPath.CPS_SUB_CA2_DER)),
                    load_cert(os.path.join(self.certs_path, CertPath.CPS_SUB_CA1_DER)),
                ]
            ),
        )

        cert_install_res = CertificateInstallationRes(
            response_code=ResponseCode.OK,
            cps_cert_chain=cps_certificate_chain,
            contract_cert_chain=contract_cert_chain,
            encrypted_private_key=encrypted_priv_key,
            dh_public_key=dh_public_key,
            emaid=emaid,
        )

        try:
            # Elements to sign, containing its id and the exi encoded stream
            contract_cert_tuple = (
                cert_install_res.contract_cert_chain.id,
                EXI().to_exi(
                    cert_install_res.contract_cert_chain, Namespace.ISO_V2_MSG_DEF
                ),
            )
            encrypted_priv_key_tuple = (
                cert_install_res.encrypted_private_key.id,
                EXI().to_exi(
                    cert_install_res.encrypted_private_key, Namespace.ISO_V2_MSG_DEF
                ),
            )
            dh_public_key_tuple = (
                cert_install_res.dh_public_key.id,
                EXI().to_exi(cert_install_res.dh_public_key, Namespace.ISO_V2_MSG_DEF),
            )
            emaid_tuple = (
                cert_install_res.emaid.id,
                EXI().to_exi(cert_install_res.emaid, Namespace.ISO_V2_MSG_DEF),
            )

            elements_to_sign = [
                contract_cert_tuple,
                encrypted_priv_key_tuple,
                dh_public_key_tuple,
                emaid_tuple,
            ]
            # The private key to be used for the signature
            signature_key = load_priv_key(
                os.path.join(self.certs_path, KeyPath.CPS_LEAF_PEM),
                KeyEncoding.PEM,
                os.path.join(self.certs_path, KeyPasswordPath.CPS_LEAF_KEY_PASSWORD),
            )

            signature = create_signature(elements_to_sign, signature_key)

        except PrivateKeyReadError as exc:
            raise Exception(
                "Can't read private key needed to create signature "
                f"for CertificateInstallationRes: {exc}",
            )
        except Exception as exc:
            raise Exception(f"Error creating signature {exc}")

        header = MessageHeaderV2(
            session_id=cert_install_req.header.session_id,
            signature=signature,
        )
        body = Body.parse_obj({"CertificateInstallationRes": cert_install_res.dict()})
        to_be_exi_encoded = V2GMessageV2(header=header, body=body)
        exi_encoded_cert_installation_res = EXI().to_exi(
            to_be_exi_encoded, Namespace.ISO_V2_MSG_DEF
        )

        base64_encode_cert_install_res = base64.b64encode(
            exi_encoded_cert_installation_res
        ).decode("utf-8")

        return base64_encode_cert_install_res


def certificate_signed_response(csr: crypto.X509Req):
    certs_path: str = Path(__file__).parent.resolve() / "everest-aux/certs/"
    ca_cert_file = certs_path / "ca/v2g/V2G_ROOT_CA.pem"
    ca_key_file = certs_path / "client/v2g/V2G_ROOT_CA.key"

    with open(ca_cert_file, "rb") as ca_cert_file, open(
        ca_key_file, "rb"
    ) as ca_key_file:
        ca_cert_data = ca_cert_file.read()
        ca_key_data = ca_key_file.read()

    ca_cert = crypto.load_certificate(crypto.FILETYPE_PEM, ca_cert_data)
    ca_key = crypto.load_privatekey(crypto.FILETYPE_PEM, ca_key_data, b"123456")

    signed_cert = crypto.X509()
    signed_cert.set_version(3)
    signed_cert.set_serial_number(1)

    signed_cert.set_subject(csr.get_subject())
    signed_cert.set_issuer(ca_cert.get_subject())
    signed_cert.set_pubkey(csr.get_pubkey())

    validity_days = 365
    not_before = datetime.utcnow()
    not_after = not_before + timedelta(days=validity_days)

    signed_cert.set_notBefore(not_before.strftime("%Y%m%d%H%M%SZ").encode("utf-8"))
    signed_cert.set_notAfter(not_after.strftime("%Y%m%d%H%M%SZ").encode("utf-8"))

    signed_cert.sign(ca_key, "sha256")

    return crypto.dump_certificate(crypto.FILETYPE_PEM, signed_cert).decode("utf-8")


def on_data_transfer(accept_pnc_authorize, **kwargs):
    req = call.DataTransferPayload(**kwargs)
    if req.vendor_id == "org.openchargealliance.iso15118pnc":
        if req.message_id == "Authorize":
            if accept_pnc_authorize:
                status = AuthorizationStatusType.accepted
            else:
                status = AuthorizationStatusType.invalid
            response = call_result201.AuthorizePayload(
                id_token_info=IdTokenInfoType(status=status)
            )
            return call_result.DataTransferPayload(
                status=DataTransferStatus.accepted,
                data=json.dumps(remove_nones(snake_to_camel_case(asdict(response)))),
            )
        # Should not be part of DataTransfer.req from CP->CSMS
        elif req.message_id == "CertificateSigned":
            return call_result.DataTransferPayload(
                status=DataTransferStatus.unknown_message_id, data="Please implement me"
            )
        # Should not be part of DataTransfer.req from CP->CSMS
        elif req.message_id == "DeleteCertificate":
            return call_result.DataTransferPayload(
                status=DataTransferStatus.unknown_message_id, data="Please implement me"
            )
        elif req.message_id == "Get15118EVCertificate":
            certs_path: str = Path(__file__).parent.resolve() / "everest-aux/certs/"
            generator: EXIGenerator = EXIGenerator(certs_path)
            exi_request = json.loads(kwargs["data"])["exiRequest"]
            namespace = json.loads(kwargs["data"])["iso15118SchemaVersion"]
            return call_result.DataTransferPayload(
                status=DataTransferStatus.accepted,
                data=json.dumps(
                    remove_nones(
                        snake_to_camel_case(
                            asdict(
                                call_result201.Get15118EVCertificatePayload(
                                    status=Iso15118EVCertificateStatusType.accepted,
                                    exi_response=generator.generate_certificate_installation_res(
                                        exi_request, namespace
                                    ),
                                )
                            )
                        )
                    )
                ),
            )
        elif req.message_id == "GetCertificateStatus":
            return call_result.DataTransferPayload(
                status=DataTransferStatus.accepted,
                data=json.dumps(
                    remove_nones(
                        snake_to_camel_case(
                            asdict(
                                call_result201.GetCertificateStatusPayload(
                                    status=GetCertificateStatusType.accepted,
                                    ocsp_result="anwfdiefnwenfinfinef",
                                )
                            )
                        )
                    )
                ),
            )
        # Should not be part of DataTransfer.req from CP->CSMS
        elif req.message_id == "InstallCertificate":
            return call_result.DataTransferPayload(
                status=DataTransferStatus.unknown_message_id, data="Please implement me"
            )
        elif req.message_id == "SignCertificate":
            return call_result.DataTransferPayload(
                status=DataTransferStatus.accepted,
                data=json.dumps(
                    asdict(
                        call_result201.SignCertificatePayload(
                            status=GenericStatusType.accepted
                        )
                    )
                ),
            )
        # Should not be part of DataTransfer.req from CP->CSMS
        elif req.message_id == "TriggerMessage":
            return call_result.DataTransferPayload(
                status=DataTransferStatus.unknown_message_id, data="Please implement me"
            )
        else:
            return call_result.DataTransferPayload(
                status=DataTransferStatus.unknown_message_id, data="Please implement me"
            )
    else:
        return call_result.DataTransferPayload(
            status=DataTransferStatus.unknown_vendor_id, data="Please implement me"
        )


@on(Action.DataTransfer)
def on_data_transfer_accept_authorize(**kwargs):
    return on_data_transfer(accept_pnc_authorize=True, **kwargs)


@on(Action.DataTransfer)
def on_data_transfer_reject_authorize(**kwargs):
    return on_data_transfer(accept_pnc_authorize=False, **kwargs)


def get_everest_config_path_str(config_name):
    return (Path(__file__).parent / "everest-aux" / "config" / config_name).as_posix()


def get_everest_config(function_name, module_name):
    if module_name == "plug_and_charge_tests":
        return Path(__file__).parent / Path(
            "everest-aux/config/everest-config-sil-iso.yaml"
        )
    elif module_name in [
        "provisioning",
        "authorization",
        "remote_control",
        "security",
        "local_authorization_list",
        "transactions",
        "meterValues",
    ]:
        return Path(__file__).parent / Path(
            "everest-aux/config/everest-config-ocpp201.yaml"
        )
    else:
        return Path(__file__).parent / Path(
            "everest-aux/config/everest-config-sil-ocpp.yaml"
        )


def test_config(request):
    data = json.loads((Path(__file__).parent / "test_config.json").read_text())

    ocpp_test_config = OcppTestConfiguration(
        charge_point_info=ChargePointInfo(**data["charge_point_info"]),
        authorization_info=AuthorizationInfo(**data["authorization_info"]),
        certificate_info=CertificateInfo(**data["certificate_info"]),
        firmware_info=FirmwareInfo(**data["firmware_info"]),
    )

    ocpp_test_config.certificate_info.csms_cert = (
        Path(__file__).parent / ocpp_test_config.certificate_info.csms_cert
    )
    ocpp_test_config.certificate_info.csms_key = (
        Path(__file__).parent / ocpp_test_config.certificate_info.csms_key
    )
    ocpp_test_config.certificate_info.csms_root_ca = (
        Path(__file__).parent / ocpp_test_config.certificate_info.csms_root_ca
    )
    ocpp_test_config.certificate_info.csms_root_ca_invalid = (
        Path(__file__).parent / ocpp_test_config.certificate_info.csms_root_ca_invalid
    )
    ocpp_test_config.certificate_info.csms_root_ca_key = (
        Path(__file__).parent / ocpp_test_config.certificate_info.csms_root_ca_key
    )
    ocpp_test_config.certificate_info.mf_root_ca = (
        Path(__file__).parent / ocpp_test_config.certificate_info.mf_root_ca
    )

    ocpp_test_config.firmware_info.update_file = (
        Path(__file__).parent / ocpp_test_config.firmware_info.update_file
    )
    ocpp_test_config.firmware_info.update_file_signature = (
        Path(__file__).parent / ocpp_test_config.firmware_info.update_file_signature
    )

    return ocpp_test_config


async def call_test_function_and_wait(test_function: FunctionType, timeout=20) -> bool:
    q = queue.Queue()

    def tst(q):
        res = test_function(timeout)
        q.put(res)

    test_thread = threading.Thread(target=tst, kwargs={"q": q})
    test_thread.start()

    result = False
    while q.empty():
        await asyncio.sleep(1)

    result = q.get()

    return result


class CertificateHashDataGenerator:
    """
    Compute the hash values for certificates.

    Note: EVSE Security uses the X509_pubkey_digest OpenSSL function for this.

    The hashes are not generated from the whole DER-encoded "Subject Public Key Information"
    field, but rather only from the bit-string representing the actual key bits (without the ASN.1
    tag and length).

    Unfortunately, there doesn't seem to be a generic method for
    doing this, so RSA and ECDSA keys are handled differently.
    If we need to add support for Ed25519 or others, we'd need to
    extend the logic here as well.

    Cf:
    - https://groups.google.com/g/mailing.openssl.users/c/1hhY2uECxsc
    - https://github.com/openssl/openssl/issues/8777
    - https://datatracker.ietf.org/doc/html/rfc5480

    """

    @staticmethod
    def _sha256(b: bytes) -> str:
        return hashlib.sha256(b).hexdigest()

    @classmethod
    def get_hash_data(
        cls, certificate_path: Path, issuer_certificate_path: Optional[Path] = None
    ):
        issuer_certificate_path = (
            issuer_certificate_path if issuer_certificate_path else certificate_path
        )

        certificate = load_pem_x509_certificate(
            certificate_path.read_bytes(), default_backend()
        )
        issuer_certificate = load_pem_x509_certificate(
            issuer_certificate_path.read_bytes(), default_backend()
        )

        issuer_name_hash = cls._get_name_hash(issuer_certificate)
        issuer_key_hash = cls._get_public_key_hash(issuer_certificate_path)

        assert issuer_name_hash == cls._get_issuer_name_hash(certificate)

        return {
            "hash_algorithm": "SHA256",
            "issuer_key_hash": issuer_key_hash,
            "issuer_name_hash": issuer_name_hash,
            "serial_number": hex(certificate.serial_number)[2:].lower(),
            # strip 0x according to OCPP spec (CertificateHashDataType)
        }

    @classmethod
    def _get_public_key_hash(cls, file: Path):
        certificate = load_pem_x509_certificate(file.read_bytes(), default_backend())
        # Get the raw key bytes - the method to do this differs by key type
        # try RSA
        try:
            return cls._sha256(
                certificate.public_key().public_bytes(
                    encoding=serialization.Encoding.DER,
                    format=serialization.PublicFormat.PKCS1,
                )
            )
        # try ECDSA (Note: We assume we're working with the uncompressed-point format here)
        except Exception:
            return cls._sha256(
                certificate.public_key().public_bytes(
                    encoding=serialization.Encoding.X962,
                    format=serialization.PublicFormat.UncompressedPoint,
                )
            )
        # if ECDSA also fails, then we need to adjust this method to add more options (e.g. Ed25519)

    @classmethod
    def _get_name_hash(cls, certificate: x509.Certificate):
        return cls._sha256(certificate.subject.public_bytes())

    @classmethod
    def _get_issuer_name_hash(cls, certificate: x509.Certificate):
        return cls._sha256(certificate.issuer.public_bytes())


class CertificateHelper:

    @staticmethod
    def _verify_private_key_matches_cert(private_key: crypto.PKey, cert: crypto.X509):
        cert_public_key = (
            cert.get_pubkey()
            .to_cryptography_key()
            .public_bytes(
                encoding=serialization.Encoding.DER,
                format=serialization.PublicFormat.SubjectPublicKeyInfo,
            )
        )
        pkey_public_key = (
            private_key.to_cryptography_key()
            .public_key()
            .public_bytes(
                encoding=serialization.Encoding.DER,
                format=serialization.PublicFormat.SubjectPublicKeyInfo,
            )
        )

        assert (
            cert_public_key == pkey_public_key
        ), f"Private key is for {pkey_public_key}; certificat has public key {pkey_public_key}"

    @classmethod
    def generate_certificate_request(
        cls, common_name: str, passphrase: str | bytes | None = None
    ) -> tuple[str, str]:
        """
        Returns: tuple of certificate request and private key
        """

        key = crypto.PKey()
        key.generate_key(crypto.TYPE_RSA, 2048)
        req = crypto.X509Req()
        req.get_subject().CN = common_name
        req.set_pubkey(key)
        req.get_subject().C = "DE"
        req.sign(key, "sha256")
        csr_data = crypto.dump_certificate_request(crypto.FILETYPE_PEM, req)
        private_key = crypto.dump_privatekey(
            crypto.FILETYPE_PEM,
            pkey=key,
            cipher="aes256" if passphrase else None,
            passphrase=(
                passphrase.encode("utf-8")
                if isinstance(passphrase, str)
                else passphrase
            ),
        )
        return csr_data.decode("utf-8"), private_key.decode("utf-8")

    @classmethod
    def sign_certificate_request(
        cls,
        csr_data: str | bytes,
        issuer_certificate_path: Path,
        issuer_private_key_path: Path,
        issuer_private_key_passphrase: str | bytes | None = None,
        relative_valid_time: int = 0,
        relative_expiration_time: int = 9999999,
        serial: int = 42,
    ) -> str:

        if isinstance(issuer_private_key_passphrase, str):
            issuer_private_key_passphrase = issuer_private_key_passphrase.encode(
                "utf-8"
            )
        if isinstance(csr_data, str):
            csr_data = csr_data.encode("utf-8")

        issuer_private_key = crypto.load_privatekey(
            crypto.FILETYPE_PEM,
            issuer_private_key_path.read_bytes(),
            passphrase=issuer_private_key_passphrase,
        )
        issuer_cert = crypto.load_certificate(
            crypto.FILETYPE_PEM, issuer_certificate_path.read_bytes()
        )

        cls._verify_private_key_matches_cert(issuer_private_key, issuer_cert)

        csr = crypto.load_certificate_request(crypto.FILETYPE_PEM, csr_data)

        # Create a new certificate
        cert = crypto.X509()
        cert.set_subject(csr.get_subject())
        cert.set_pubkey(csr.get_pubkey())
        cert.gmtime_adj_notBefore(
            min(relative_valid_time, relative_expiration_time - 1)
        )
        cert.gmtime_adj_notAfter(relative_expiration_time)
        cert.set_issuer(issuer_cert.get_subject())
        cert.set_serial_number(serial)
        cert.sign(issuer_private_key, "SHA256")
        signed_certificate = crypto.dump_certificate(crypto.FILETYPE_PEM, cert)

        return signed_certificate.decode(encoding="utf-8")


class OCPPConfigReader:

    def __init__(self, config):
        self._config_json = config

    def get_variable(self, section: str, variable: str):
        identifier = OCPP201ConfigVariableIdentifier(section, variable)

        return GenericOCPP201ConfigAdjustment._get_value_from_v201_config(
            self._config_json, identifier
        )
