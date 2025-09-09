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
| AC, AC_BPT                         | :heavy_check_mark: |
| MCS (Amd.)                         | :heavy_check_mark: |
| AC DER (Amd.)                      |                    |
| WPT                                |                    |
| ACDP                               |                    |
| ExternalPayment                    | :heavy_check_mark: |
| Plug&Charge                        | WIP                |
| CertificateInstallation            |                    |
| Scheduled Mode                     | :heavy_check_mark: |
| Dynamic Mode (+ MobilityNeedsMode) | :heavy_check_mark: |
| Private Env                        |                    |
| Pause/Resume                       | :heavy_check_mark: (dynamic mode) |
| Standby                            |                    |
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

To build this library you need [everest-cmake](https://github.com/EVerest/everest-cmake) checkout in the same directory as libiso15118. If no `everest-cmake` is available, it is retrieved via FetchContent.

For Debian GNU/Linux 12 you will need the following dependencies:

```bash
sudo apt update
sudo apt install build-essential cmake libssl-dev
```

For Fedora 41+ you will need the following dependencies:

```bash
sudo dnf update
sudo dnf install gcc gcc-c++ git make cmake openssl-devel 
```

OpenSSL version 3.0 or above is required. The build system `ninja` is optional. 

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

GDB Debugging (VS Code)
-----------------------

Run the cmake (from the build dir) with the debug commands on:
```
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
```

Run the GDB debugger with the following configuration:
```
{
    "name": "(gdb) Launch LIBISO TESTS",
    "type": "cppdbg",
    "request": "launch",
    "program": "${workspaceFolder}/libiso15118/build/test/iso15118/fsm/test_d20_transitions",
    "args": [],
    "stopAtEntry": false,
    "cwd": "${workspaceFolder}/libiso15118/build/test/iso15118/fsm/",
    "environment": [],
    "externalConsole": false,
    "MIMode": "gdb",
    "setupCommands": [
        {
            "description": "Enable pretty-printing for gdb",
            "text": "-enable-pretty-printing",
            "ignoreFailures": true
        },
        {
            "description": "Set Disassembly Flavor to Intel",
            "text": "-gdb-set disassembly-flavor intel",
            "ignoreFailures": true
        }
    ]
},
```

Replace the `program` path to any test executable you are debugging.

Acknowledgment
--------------

This library has thankfully received support from the German Federal Ministry
for Economic Affairs and Climate Action.
Information on the corresponding research project can be found here (in
German only):
[InterBDL research project](https://www.thu.de/de/org/iea/smartgrids/Seiten/InterBDL.aspx).

![Logo of funding by Federal Ministry of Economic Affairs and Climate Action](https://raw.githubusercontent.com/EVerest/EVerest/main/docs/img/bmwk-logo-incl-supporting.png)
