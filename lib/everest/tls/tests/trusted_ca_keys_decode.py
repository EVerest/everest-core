#!/usr/bin/env python3

"""
trusted ca keys hex string
"0069011d484406bca8888997a7462416445e7db117114c017f204de30f1cd42c9e6dae91b6a8ac9b8d481ba601597be7013ad6fc397b78b01d90cea1b7f909f145011d484406bca8888997a7462416445e7db117114c0100fae3900795c888a4d4d7bd9fdffa60418ac19f"

length 0069
"01 1d484406bca8888997a7462416445e7db117114c"
"01 7f204de30f1cd42c9e6dae91b6a8ac9b8d481ba6"
"01 597be7013ad6fc397b78b01d90cea1b7f909f145"
"01 1d484406bca8888997a7462416445e7db117114c"
"01 00fae3900795c888a4d4d7bd9fdffa60418ac19f"

key hash from certificate
openssl x509 -in cert.pem -pubkey -noout | openssl enc -base64 -d | openssl dgst -sha1
"""

from cryptography import x509
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives import hashes

import argparse


def certificate_key_hash(filename):
    with open(filename, "rb") as fp:
        cert = x509.load_pem_x509_certificate(fp.read())
        pub = cert.public_key()
        pub_der = pub.public_bytes(
            encoding=serialization.Encoding.DER,
            format=serialization.PublicFormat.SubjectPublicKeyInfo,
        )
        dgst = hashes.Hash(hashes.SHA1())
        dgst.update(pub_der)
        sha1 = dgst.finalize()
        print(sha1.hex())


def certificate_hash(filename):
    # note this is the hash of the whole certificate including signature
    with open(filename, "rb") as fp:
        cert = x509.load_pem_x509_certificate(fp.read())
        pub_der = cert.public_bytes(encoding=serialization.Encoding.DER)
        dgst = hashes.Hash(hashes.SHA1())
        dgst.update(pub_der)
        sha1 = dgst.finalize()
        print(sha1.hex())


def trusted_ca_keys_decode(data):
    data_len = int.from_bytes(data[:2], "big", signed=False)
    data = data[2:]
    assert len(data) == data_len
    while data:
        entry_type = data[0]
        data = data[1:]
        if entry_type == 0:
            print("pre_agreed")
        elif entry_type == 1:
            sha1 = data[:20]
            data = data[20:]
            print("key_sha1_hash:  %s" % sha1.hex())
        elif entry_type == 2:
            print("x509_name (not decoded yet)")
        elif entry_type == 3:
            sha1 = data[:20]
            data = data[20:]
            print("cert_sha1_hash: %s" % sha1.hex())


def trusted_ca_keys_decode_file(filename):
    with open(filename, "rb") as fp:
        trusted_ca_keys_decode(fp.read())


def trusted_ca_keys_decode_hex(hexstr):
    trusted_ca_keys_decode(bytes.fromhex(hexstr))


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--key",
        action="store",
        help="filename: Print sha1 hash of certificate public key",
    )
    parser.add_argument(
        "--cert", action="store", help="filename: Print sha1 hash of certificate"
    )
    parser.add_argument(
        "--file", action="store", help="filename: Parse trusted ca keys"
    )
    parser.add_argument(
        "--hex", action="store", help="parse trusted ca keys hex string"
    )

    args = parser.parse_args()
    if args.key:
        certificate_key_hash(args.key)
    if args.cert:
        certificate_hash(args.cert)
    if args.file:
        trusted_ca_keys_decode_file(args.file)
    if args.hex:
        trusted_ca_keys_decode_hex(args.hex)
