#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#
# heavily based on create_certs.sh from https://github.com/SwitchEV/iso15118

import argparse
import subprocess
import shutil
from pathlib import Path
import tempfile
from typing import NamedTuple

OPENSSL_EXECUTABLE_NAME = 'openssl'


def create_temporary_file_with_content(data: bytes) -> Path:
    with tempfile.NamedTemporaryFile(delete=False) as fp:
        fp.write(data)
        return Path(fp.name)


def create_parent_directory_for_file(path: Path):
    path.parent.mkdir(parents=True, exist_ok=True)


class CryptoParameters(NamedTuple):
    symmetric_cipher: str
    symmetric_cipher_pks12: str
    sha: str
    ec_curve: str


class PrivateKey:
    def __init__(self, data: bytes, password: str):
        self._data = data
        self._password = password
        self._path: Path = None

    def persist(self, path: Path):
        path = path.resolve()
        create_parent_directory_for_file(path)
        path.write_bytes(self._data)
        self._path = path

    @property
    def is_persisted(self):
        return self._path != None

    @property
    def path(self):
        return self._path

    @property
    def password(self):
        return self._password


class CertificateConfigSection(NamedTuple):
    name: str
    entries: dict[str, str]

    def __str__(self):
        header = f'[{self.name}]'
        entries = [f'{k} = {v}' for k, v in self.entries.items()]
        return '\n'.join([header] + entries) + '\n'


class CertificateConfig(NamedTuple):
    sections: list[CertificateConfigSection]

    @property
    def data(self):
        result = '\n'.join([str(e) for e in self.sections])
        return result.encode()


class CreateCertificateJob(NamedTuple):
    prefix: str
    certificate_config: CertificateConfig
    days_valid: int = 60
    password: str = '123456'

    # FIXME (aw): this is potentially dangerous as we have to assume, that no two jobs have the same prefix
    def __hash__(self) -> int:
        return hash(self.prefix)

    def __eq__(self, other):
        return self.prefix == other.prefix


class CreatedCertificate(NamedTuple):
    key: PrivateKey
    certificate: Path


class CertificateChainJob(NamedTuple):
    certificates: list[CreateCertificateJob]
    has_leaf: bool


def create_certificate_config(cn: str, dc: str, ext: dict[str, str]):
    return CertificateConfig(sections=[
        CertificateConfigSection('req', {
            'prompt': 'no',
            'distinguished_name': 'ca_dn'
        }),
        CertificateConfigSection('ca_dn', {
            'commonName': cn,
            'organizationName': 'Pionix',
            'countryName': 'DE',
            'domainComponent': dc
        }),
        CertificateConfigSection('ext', {'subjectKeyIdentifier': 'hash', **ext}),
    ])


class CertificateGenerator:

    def __init__(self, crypto_parameters: CryptoParameters):
        self._openssl_executable = shutil.which(OPENSSL_EXECUTABLE_NAME)

        self._crypto_parameters = crypto_parameters

        if not self._openssl_executable:
            raise Exception(f'Could not find openssl executable "{OPENSSL_EXECUTABLE_NAME}"')

    def create_private_key(self, password: str):
        args = ["ecparam",
                "-genkey",
                "-name", self._crypto_parameters.ec_curve
                ]
        ec_params = self._run_openssl(args)

        args = ['ec', self._crypto_parameters.symmetric_cipher,
                '-passout', f'pass:{password}'
                ]
        key = self._run_openssl(args, input=ec_params)

        return PrivateKey(data=key, password=password)

    def create_certificate_signing_request(self, config: Path, private_key: PrivateKey):
        args = ['req', '-new',
                '-key', str(private_key.path),
                '-passin', f'pass:{private_key.password}',
                '-config', str(config)]
        csr = self._run_openssl(args)
        return csr

    def create_signed_certificate(self, csr: bytes, extension_file: Path, signing_key: PrivateKey, ca_certificate: Path = None, days_valid: int = 60):
        args = ['x509', '-req',
                '-extfile', str(extension_file),
                '-extensions', 'ext',
                '-days', str(days_valid),
                '-set_serial', self._get_random_serial()
                ]

        if not ca_certificate:
            # self signing
            args += [
                '-signkey', str(signing_key.path),
                '-passin', f'pass:{signing_key.password}',
                self._crypto_parameters.sha
            ]
        else:
            args += [
                '-CA', str(ca_certificate),
                '-CAkey', str(signing_key.path),
                '-passin', f'pass:{signing_key.password}'
            ]

        cert = self._run_openssl(args, input=csr)
        return cert

    def _run_openssl(self, arguments: list[str], input: bytes = None) -> bytes:
        cmdline = [self._openssl_executable] + arguments
        res = subprocess.run(cmdline,  input=input, capture_output=True)

        if res.returncode:
            error_message = 'Command [ ' + ' '.join(cmdline) + f' ] failed with error code {res.returncode}'
            error_message += f':\n{res.stderr.decode()}'
            raise Exception(error_message)

        return res.stdout

    def _get_random_serial(self):
        return '0x' + self._run_openssl(['rand', '-hex', '8']).decode().rstrip()


class ChainProcessor:
    def __init__(self, generator: CertificateGenerator, path):
        self._path = path
        self._gen = generator
        self._reg: dict[str, CreatedCertificate] = {}

    def __call__(self, job: CertificateChainJob):
        previous: CreatedCertificate = None

        for index, cert_job in enumerate(job.certificates):
            is_leaf = index == len(job.certificates) - 1 and job.has_leaf

            previous = self._create_certificate(cert_job, previous, is_leaf)

    def _create_certificate(self, job: CreateCertificateJob, signing_cert: CreatedCertificate, is_leaf: bool):
        cache = self._reg.get(job.prefix, None)

        if cache:
            if not signing_cert:
                # needs to be a root one
                return cache
            else:
                raise Exception(f'Certificate with prefix {job.prefix} has been already created and is not a root')

        # first, generate the key
        private_key = self._gen.create_private_key(job.password)
        private_key.persist(self._get_key_path(job))

        # create temporary for config
        tmp_cert_config = create_temporary_file_with_content(job.certificate_config.data)

        try:
            # create signing request
            csr_data = self._gen.create_certificate_signing_request(tmp_cert_config, private_key)

            signing_key = signing_cert.key if signing_cert else private_key
            ca_certificate = signing_cert.certificate if signing_cert else None

            # create certificate
            cert_data = generator.create_signed_certificate(
                csr_data, tmp_cert_config, signing_key, ca_certificate, job.days_valid)
            cert_path = self._get_cert_path(job, is_leaf)
            create_parent_directory_for_file(cert_path)
            cert_path.write_bytes(cert_data)

            cert = CreatedCertificate(private_key, cert_path)
            self._reg[job.prefix] = cert
            return cert

        finally:
            tmp_cert_config.unlink()

    def _get_key_path(self, job: CreateCertificateJob):
        return self._path / 'client' / f'{job.prefix}.key'

    def _get_cert_path(self, job: CreateCertificateJob, is_leaf: bool):
        return self._path / ('client' if is_leaf else 'ca') / f'{job.prefix}.pem'


if __name__ == "__main__":
    v2g_root = CreateCertificateJob(
        prefix='v2g/V2G_ROOT_CA',
        certificate_config=create_certificate_config('V2GRootCA', 'V2G', {
            'basicConstraints': 'critical,CA:true',
            'keyUsage': 'critical,keyCertSign,cRLSign'
        })
    )

    cpo_sub_ca1 = CreateCertificateJob(
        prefix='csms/CPO_SUB_CA1',
        certificate_config=create_certificate_config('CPOSubCA1', 'V2G', {
            'basicConstraints': 'critical,CA:true,pathlen:1',
            'keyUsage': 'critical,keyCertSign,cRLSign',
            'authorityInfoAccess': 'OCSP;URI:https://www.example.com/,caIssuers;URI:https://www.example.com/Intermediate-CA.cer'
        })
    )

    cpo_sub_ca2 = CreateCertificateJob(
        prefix='csms/CPO_SUB_CA2',
        certificate_config=create_certificate_config('CPOSubCA2', 'V2G', {
            'basicConstraints': 'critical,CA:true,pathlen:0',
            'keyUsage': 'critical,keyCertSign,cRLSign',
            'authorityInfoAccess': 'OCSP;URI:https://www.example.com/,caIssuers;URI:https://www.example.com/Intermediate-CA.cer'
        })
    )

    secc_leaf = CreateCertificateJob(
        prefix='cso/SECC_LEAF',
        certificate_config=create_certificate_config('SECCCert', 'CPO', {
            'basicConstraints': 'critical,CA:false',
            'keyUsage': 'critical,digitalSignature,keyAgreement',
        })
    )

    cps_sub_ca1 = CreateCertificateJob(
        prefix='cps/CPS_SUB_CA1',
        certificate_config=create_certificate_config('ProvSubCA1', 'CPS', {
            'basicConstraints': 'critical,CA:true,pathlen:1',
            'keyUsage': 'critical,keyCertSign,cRLSign',
        })
    )

    cps_sub_ca2 = CreateCertificateJob(
        prefix='cps/CPS_SUB_CA2',
        certificate_config=create_certificate_config('ProvSubCA2', 'CPS', {
            'basicConstraints': 'critical,CA:true,pathlen:0',
            'keyUsage': 'critical,keyCertSign,cRLSign',
        })
    )

    cps_leaf = CreateCertificateJob(
        prefix='cps/CPS_LEAF',
        certificate_config=create_certificate_config('CPS Leaf', 'CPS', {
            'basicConstraints': 'critical,CA:false',
            'keyUsage': 'critical,digitalSignature',
        })
    )

    mo_root = CreateCertificateJob(
        prefix='mo/MO_ROOT_CA',
        certificate_config=create_certificate_config('MORootCA', 'MO', {
            'basicConstraints': 'critical,CA:true',
            'keyUsage': 'critical,keyCertSign,cRLSign',
            'authorityInfoAccess': 'OCSP;URI:https://www.example.com/,caIssuers;URI:https://www.example.com/Intermediate-CA.cer'
        })
    )

    mo_sub_ca1 = CreateCertificateJob(
        prefix='mo/MO_SUB_CA1',
        certificate_config=create_certificate_config('PKI-Ext_CRT_MO_SUB1_VALID', 'MO', {
            'basicConstraints': 'critical,CA:true,pathlen:1',
            'keyUsage': 'critical,keyCertSign,cRLSign',
            'authorityInfoAccess': 'OCSP;URI:https://www.example.com/,caIssuers;URI:https://www.example.com/Intermediate-CA.cer'
        })
    )

    mo_sub_ca2 = CreateCertificateJob(
        prefix='mo/MO_SUB_CA2',
        certificate_config=create_certificate_config('PKI-Ext_CRT_MO_SUB2_VALID', 'MO', {
            'basicConstraints': 'critical,CA:true,pathlen:0',
            'keyUsage': 'critical,digitalSignature,nonRepudiation,keyCertSign,cRLSign',
            'authorityInfoAccess': 'OCSP;URI:https://www.example.com/,caIssuers;URI:https://www.example.com/Intermediate-CA.cer'
        })
    )

    mo_leaf = CreateCertificateJob(
        prefix='mo/MO_LEAF',
        certificate_config=create_certificate_config('UKSWI123456789A', 'MO', {
            'basicConstraints': 'critical,CA:false',
            'keyUsage': 'critical,digitalSignature,nonRepudiation,keyEncipherment,keyAgreement',
            'authorityInfoAccess': 'OCSP;URI:https://www.example.com/,caIssuers;URI:https://www.example.com/Intermediate-CA.cer'
        })
    )

    oem_root = CreateCertificateJob(
        prefix='oem/OEM_ROOT_CA',
        certificate_config=create_certificate_config('OEMRootCA', 'OEM', {
            'basicConstraints': 'critical,CA:true',
            'keyUsage': 'critical,keyCertSign,cRLSign',
        })
    )

    oem_sub_ca1 = CreateCertificateJob(
        prefix='oem/OEM_SUB_CA1',
        certificate_config=create_certificate_config('OEMSubCA1', 'OEM', {
            'basicConstraints': 'critical,CA:true,pathlen:1',
            'keyUsage': 'critical,keyCertSign,cRLSign',
        })
    )

    oem_sub_ca2 = CreateCertificateJob(
        prefix='oem/OEM_SUB_CA2',
        certificate_config=create_certificate_config('OEMSubCA2', 'OEM', {
            'basicConstraints': 'critical,CA:true,pathlen:0',
            'keyUsage': 'critical,keyCertSign,cRLSign',
        })
    )

    oem_leaf = CreateCertificateJob(
        prefix='oem/OEM_LEAF',
        certificate_config=create_certificate_config('OEMProvCert', 'OEM', {
            'basicConstraints': 'critical,CA:false',
            'keyUsage': 'critical,digitalSignature,keyAgreement',
        })
    )

    chains = [
        CertificateChainJob([oem_root, oem_sub_ca1, oem_sub_ca2, oem_leaf], True),  # oem chain
        CertificateChainJob([mo_root, mo_sub_ca1, mo_sub_ca2, mo_leaf], True),  # mo chain
        CertificateChainJob([v2g_root, cps_sub_ca1, cps_sub_ca2, cps_leaf], True),  # cps chain
        CertificateChainJob([v2g_root, cpo_sub_ca1, cpo_sub_ca2, secc_leaf], True),  # cpo chain
    ]

    parser = argparse.ArgumentParser(description='Generating certificates for V2G structure')
    parser.add_argument('--output-dir', type=Path, default=Path('certs'))
    args = parser.parse_args()

    generator = CertificateGenerator(
        CryptoParameters(
            symmetric_cipher='-aes-128-cbc',
            symmetric_cipher_pks12='-aes128',
            sha='-sha256',
            ec_curve='prime256v1'
        )
    )

    processor = ChainProcessor(generator, args.output_dir)

    for chain in chains:
        processor(chain)
