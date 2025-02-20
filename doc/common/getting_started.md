# Getting Started

## Requirements

For Debian GNU/Linux 11 you will need the following dependencies:

```bash
  sudo apt install build-essential cmake python3-pip libboost-all-dev libsqlite3-dev libssl-dev
```

OpenSSL version 3.0 or above is required.

Clone this repository.

```bash
  git clone https://github.com/EVerest/libocpp
```

In the libocpp folder create a folder named build and cd into it.
Execute cmake and then make:

```bash
  mkdir build && cd build
  cmake ..
  make -j$(nproc)
```

## Unit testing

GTest is required for building the test cases target.
To build the target and run the tests you can reference the script `.ci/build-kit/install_and_test.sh`.
The script allows the GitHub Actions runner to execute.

Local testing:

```bash
mkdir build
cmake -B build -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="./dist"
cd build
make -j$(nproc) install
```

Run any required tests from build/tests.

## Clarifications for directory structures, namespaces and OCPP versions

This repository contains multiple subdirectories and namespaces named v16, v2 and v21.

* The v16 directories contain files for configuring and implementing OCPP 1.6.
* The v2 directories include files for both OCPP2.0.1 and OCPP2.1, as OCPP 2.1
is fully backward compatible with OCPP 2.0.1.
* The v21 directories include files only for OCPP2.1.

## Get Started with OCPP1.6

Please see the [Getting Started documentation for OCPP1.6](../v16/getting_started.md).

## Get Started with OCPP2.0.1

Please see the [Getting Started documentation for OCPP2.0.1](../v2/getting_started.md).

## Building the doxygen documentation

```bash
  cmake -S . -B build
  cmake --build build --target doxygen-ocpp
```

You will find the generated doxygen documentation at:
`build/dist/docs/html/index.html`

The main reference for the integration of libocpp for OCPP1.6 is the ocpp::v16::ChargePoint class defined in `v16/charge_point.hpp` , for OCPP2.0.1 and OCPP2.1 that is the ocpp::v2::ChargePoint class defined in `v2/charge_point.hpp` .
