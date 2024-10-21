# libevse-security

![Github Actions](https://github.com/EVerest/libevse-security/actions/workflows/build_and_test.yml/badge.svg)

This is a C++ library for security related operations for charging stations. It respects the requirements specified in OCPP and ISO15118 and can be used in combination with OCPP and ISO15118 implementations.

All documentation and the issue tracking can be found in our main repository here: https://github.com/EVerest/everest

## Prerequisites

The library requires OpenSSL 3.

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

**Important:** when requesting leaf certificates with [get_leaf_certificate_info](https://github.com/EVerest/libevse-security/blob/b140c17b0a5eaf09b60035605ed8aeb84627eb78/include/evse_security/evse_security.hpp#L195) care should be taken if you require the full certificate chain.

If a full chain is **Leaf->SubCA2->SubCA1->Root**, it is recommended to have the root certificate in a single file, **V2G_ROOT_CA.pem** for example. The **Leaf->SubCA2->SubCA1** should be placed in a file e.g. **SECC_CERT_CHAIN.pem**. 
  
## Certificate Signing Request

There are two configuration options that will add a DNS name and IP address to the
subject alternative name in the certificate signing request.
By default they are not added.

- `cmake -DCSR_DNS_NAME=charger.pionix.de ...` to include a DNS name
- `cmake -DCSR_IP_ADDRESS=192.168.2.1 ...` to include an IPv4 address

When receiving back a signed CSR, the library will take care to create two files, one containing the **Leaf->SubCA2->SubCA1** chain and another containing the single **Leaf**. When they both exist, the return of [get_leaf_certificate_info](https://github.com/EVerest/libevse-security/blob/b140c17b0a5eaf09b60035605ed8aeb84627eb78/include/evse_security/evse_security.hpp#L195) will contain a path to both the single file and the chain file.

## TPM Provider
There is a configuration option to configure OpenSSL for use with a TPM.<br>
`cmake` ... `-DUSING_TPM2=ON`<br>

The library will use the `UseTPM` flag and the PEM private key file to
configure whether to use the `default` provider or the `tpm2` provider.

Configuration is managed via propquery strings (see CMakeLists.txt)

- `PROPQUERY_PROVIDER_DEFAULT` is the string to use when selecting the default provider
- `PROPQUERY_PROVIDER_CUSTOM` is the string to use when selecting the tpm2 provider

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

<b>Note:</b> In case of errors related to CSR signing, update tpm2-openssl to v 1.2.0.

## Custom Provider
There is a configuration option to configure OpenSSL for use with a custom provider.<br>
`cmake` ... `-DUSING_CUSTOM_PROVIDER=ON`<br>

The workflow follows the same steps as using the TPM provider. The library will
have a flag to configure whether it uses the `default` provider or the `custom` one.

<b>Note:</b> The custom provider name has to be defined [here](https://github.com/EVerest/libevse-security/blob/4afe644cb62d0bf06fff1e2ca5d2dbc489342e0c/CMakeLists.txt#L32). Change the name from "custom_provider" to the required provider.

## Garbage Collect

By default a garbage collect function will run and delete all expired leaf certificates and their respective keys, only if the certificate storage is full. A minimum count of leaf certificates will be kept even if they are expired. 

Certificate signing requests have an expiry time. If the CSMS does not respond to them within that timeframe, CSRs will be deleted.

Defaults:
- Garbage collect time: 20 minutes
- CSR expiry: 60 minutes
- Minimum certificates kept: 10
- Maximum storage space: 50 MB
- Maximum certificate entries: 2000

## Limitations

Based on information from [ssl](https://www.ssl.com/article/what-are-root-certificates-and-why-do-they-matter/), self-signed roots are possible, but not supported in our library at the moment.

Cross-signed certificate chains (see [ssl](https://www.ssl.com/blogs/ssl-com-legacy-cross-signed-root-certificate-expiring-on-september-11-2023/)), required for seamless root transitions are not supported at the moment.