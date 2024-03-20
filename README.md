# libevse-security

![Github Actions](https://github.com/EVerest/libevse-security/actions/workflows/build_and_test.yml/badge.svg)

This is a C++ library for security related operations for charging stations. It respects the requirements specified in OCPP and ISO15118 and can be used in combination with OCPP and ISO15118 implementations.

In the near future this library will also contain support for secure storage on TPM2.0.

All documentation and the issue tracking can be found in our main repository here: https://github.com/EVerest/everest

## Prerequisites

The library requires OpenSSL 1.1.1.

## Build Instructions

Clone this repository and build with CMake.

```bash
git clone git@github.com:EVerest/libevsesecurity.git
cd libevsesecurity
mkdir build && cd build
cmake ..
make -j$(nproc) install
```

## Tests

GTest is required for building the test cases target.
To build the target and run the tests use:

```bash
mkdir build && cd build
cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=./dist ..
make -j$(nproc) install
make test
```

## Certificate Structure

We allow any certificate structure with the following recommendations:

- Root CA certificate directories/bundles should not overlap leaf certificates
- It is not recommended to store any SUBCAs in the root certificate bundle (if using files)

**Important:** when requesting leaf certificates with [get_key_pair](https://github.com/EVerest/libevse-security/blob/5cd5f8284229ffd28ae1dfed2137ef194c39e732/lib/evse_security/evse_security.cpp#L820) care should be taken if you require the full certificate chain.

If a full chain is **Leaf->SubCA2->SubCA1->Root**, it is recommended to have the root certificate in a single file, **V2G_ROOT_CA.pem** for example. The **Leaf->SubCA2->SubCA1** should be placed in a file e.g. **SECC_CERT_CHAIN.pem**. 
  
## Certificate Signing Request

There are two configuration options that will add a DNS name and IP address to the
subject alternative name in the certificate signing request.
By default they are not added.

- `cmake -DCSR_DNS_NAME=charger.pionix.de ...` to include a DNS name
- `cmake -DCSR_IP_ADDRESS=192.168.2.1 ...` to include an IPv4 address

When receiving back a signed CSR, the library will take care to create two files, one containing the **Leaf->SubCA2->SubCA1** chain and another containing the single **Leaf**. When they both exist, the return of [get_key_pair](https://github.com/EVerest/libevse-security/blob/5cd5f8284229ffd28ae1dfed2137ef194c39e732/include/evse_security/evse_types.hpp#L126) will contain a path to both the single file and the chain file.

## TPM
There is a configuration option to configure OpenSSL for use with a TPM.<br>
`cmake` ... `-DUSING_TPM2=ON`<br>
Note OpenSSL providers are not available for OpenSSL v1, OpenSSL v3 is required.

The library will use the `UseTPM` flag and the PEM private key file to
configure whether to use the `default` provider or the `tpm2` provider.

Configuration is managed via propquery strings (see CMakeLists.txt)

- `PROPQUERY_DEFAULT` is the string to use when selecting the default provider
- `PROPQUERY_TPM2` is the string to use when selecting the tpm2 provider

propquery|action
---------|------
"provider=default"|use the default provider
"provider=tpm2"|use the tpm2 provider
"provider!=tpm2"|don't use the tpm provider
"?provider=tpm2,tpm2.digest!=yes"|prefer the tpm2 provider but not for message digests

For more information see:

- [Provider for integration of TPM 2.0 to OpenSSL 3.x](https://github.com/tpm2-software/tpm2-openssl)
- [OpenSSL property](https://www.openssl.org/docs/man3.0/man7/property.html)
- [OpenSSL provider](https://www.openssl.org/docs/man3.0/man7/provider.html)

## Garbage Collect

By default a garbage collect function will run and delete all expired leaf certificates and their respective keys, only if the certificate storage is full. A minimum count of leaf certificates will be kept even if they are expired. 

Certificate signing requests have an expiry time. If the CSMS does not respond to them within that timeframe, CSRs will be deleted.

Defaults:
- Garbage collect time: 20 minutes
- CSR expiry: 60 minutes
- Minimum certificates kept: 10
- Maximum storage space: 50 MB
- Maximum certificate entries: 2000
