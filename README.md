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
