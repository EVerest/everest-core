ISO 15118 library suite
=======================

This is a C++ library implementation of ISO 15118-20, ISO15118-2 and DIN70121. The implementation of ISO15118-20 is currently under heavy development. DIN70121 and ISO15118-2 will follow and are currently covered by the [EvseV2G module](https://github.com/EVerest/everest-core/tree/main/modules/EvseV2G) of everest-core.

ISO 15118-20 Support
--------------------

The following table shows the current support for the listed EVSE ISO15118-20 features.

| Feature                            | Supported          |
|------------------------------------|--------------------|
| TCP, TLS 1.2 & 1.3                 | :heavy_check_mark: |
| DC, DC_BPT                         | :heavy_check_mark: |
| AC, AC_BPT                         | WIP                |
| WPT                                |                    |
| ACDP                               |                    |
| ExternalPayment                    | :heavy_check_mark: |
| Plug&Charge                        | WIP                |
| CertificateInstallation            |                    |
| Scheduled Mode                     | :heavy_check_mark: |
| Dynamic Mode (+ MobilityNeedsMode) | :heavy_check_mark: |
| Private Env                        |                    |
| Pause/Resume & Standby             | WIP                |
| Schedule Renegotation              |                    |
| Smart Charging                     |                    |
| Multiplex messages                 |                    |
| Internet Service                   |                    |
| Parking Status Service             |                    |

ISO 15118 Support
-----------------

ISO15118 support is distributed accross multiple repositories and modules in EVerest. Please see the following references of other ISO15118 related development:

- Some functionality of part 2 of ISO 15118 is integrated in the
  [EvseManager module in the everest-core repository](https://github.com/EVerest/everest-core/tree/main/modules/EvseManager).
- Current development for an EXI code generator (as used in the
  ISO 15118 protocol suite) is ongoing in the
  [cbexigen repository](https://github.com/EVerest/cbexigen).
- The [repository libSlac](https://github.com/EVerest/libslac) contains
  definitions of SLAC messages that are used for ISO 15118 communication.
- DIN70121 & ISO15118-2 funcationality can be found in
  [EVerest module EvseV2G](https://github.com/EVerest/everest-core/tree/main/modules/EvseV2G)

Dependencies
------------

To build this library you need [everest-cmake](https://github.com/EVerest/everest-cmake) checkout in the same directory as libiso15118.

Getting started
---------------

```
# Run cmake (BUILD_TESTING to enable/disable unit tests)
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Run cmake with disabled compiler warnings
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DISO15118_COMPILE_OPTIONS_WARNING=""

# Build
ninja -C build

# Running tests
ninja -C build test

# Generating a code coverage (BUILD_TESTING should be enabled)
ninja -C build iso15118_gcovr_coverage
```

The coverage report will be available in the index.html file in the `build/iso15118_gcovr_coverage` directory.

Version 8.2 or higher of gcovr is required for the coverage report. Install gcovr release from PyPI:
```
pip install gcovr
```
