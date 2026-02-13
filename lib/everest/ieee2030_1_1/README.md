# C++ implementation of IEEE Std 2030.1.1-2021

This is a C++ open source implementation of the IEEE Std 2030.1.1-2021 communication protocol.

> **Note:** This is a "rump library" (Work In Progress). It currently implements a subset of the full protocol stack and is not production-ready.

## Overview

`libIEEE2030.1.1` implements the digital communication parameters for DC Quick and Bidirectional Chargers as defined in **IEEE Std 2030.1.1-2021**. This standard specifies the interface for rapid energy transfer between Electric Vehicles (EVs) and DC chargers.

## Relationship to CHAdeMO

The IEEE 2030.1.1 standard is technically derived from CHAdeMO specifications with the permission of the CHAdeMO Association. Consequently, this library serves as a foundation for systems intended to interoperate with vehicles using the CHAdeMO protocol.

## Supported Versions

The library targets the following protocol definitions:
* **1.1 / 1.2**
* **2.0.1**

## Compliance and Certification

Implementation of this library does not grant compliance with CHAdeMO requirements. While the IEEE standard and CHAdeMO share core specifications, commercial chargers require official certification and adherence to CHAdeMO Association governance.

## Features

* **CAN Communication:** Handling of standard ID allocation (Vehicle: H'100-H'102, Charger: H'108-H'109).
* **Sequence Control:** Basic state machine for charge control sequences (Start, Charging, Stop).
* **Error Handling:** Definitions for standard error flags.

## Integration with EVerest

This library will be (but is not yet) integrated within the `IEEE2030.1.1` module within [everest-core](https://github.com/EVerest/everest-core) — the complete software stack for your charging station. It is recommended to use EVerest together with this IEEE 2030.1.1 implementation.

## Dependencies

To build this library you need [everest-cmake](https://github.com/EVerest/everest-cmake) checkout in the same directory as `libIEEE2030.1.1`. If no `everest-cmake` is available, it is retrieved via FetchContent.

For Debian GNU/Linux 12 you will need the following dependencies:

```bash
sudo apt update
sudo apt install build-essential cmake
```

For Fedora 42+ you will need the following dependencies:

```bash
sudo dnf update
sudo dnf install gcc gcc-c++ git make cmake
```

The build system `ninja` is optional. 

## Getting started

```
# Run cmake (BUILD_TESTING to enable/disable unit tests)
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Run cmake with disabled compiler warnings
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DIEEE2030_COMPILE_OPTIONS_WARNING=""

# Build
ninja -C build

# Running tests
ninja -C build test
```

## Get Involved

See the [COMMUNITY.md](https://github.com/EVerest/everest-core/blob/main/COMMUNITY.md) and [CONTRIBUTING.md](https://github.com/EVerest/everest-core/blob/main/CONTRIBUTING.md) of the EVerest project to get involved.

## License

This project is licensed under the Apache License 2.0. See the [LICENSE](./LICENSE) file for details.

