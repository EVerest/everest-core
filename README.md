# C++ implementation of OCPP

This is a C++ library implementation of OCPP for version 1.6 (https://www.openchargealliance.org/protocols/ocpp-16/) and 2.0.1 (https://www.openchargealliance.org/protocols/ocpp-201/). It enables charging stations to communicate with cloud backends for remote control, monitoring and billing of charging processes.

Libocpp can be used for the communication of one charging station and multiple EVSE using a single websocket connection.

Libocpp provides a complete implementation of OCPP 1.6. The implementation of OCPP 2.0.1 is currently under development.
## Feature Support

The following tables show the current support for the listed feature profiles / functional blocks and application notes.

All documentation and the issue tracking can be found in our main repository here: https://github.com/EVerest/

### Feature Profile Support OCPP 1.6

| Feature Profile            | Supported                 |
| -------------------------- | ------------------------- |
| Core                       | :heavy_check_mark: yes    |
| Firmware Management        | :heavy_check_mark: yes    |
| Local Auth List Management | :heavy_check_mark: yes    |
| Reservation                | :heavy_check_mark: yes    |
| Smart Charging             | :heavy_check_mark: yes    |
| Remote Trigger             | :heavy_check_mark: yes    |

| Whitepapers                                                                                                                               | Supported              |
| ----------------------------------------------------------------------------------------------------------------------------------------- | ---------------------- |
| [OCPP 1.6 Security Whitepaper (3rd edition)](https://www.openchargealliance.org/uploads/files/OCPP-1.6-security-whitepaper-edition-3.zip) | :heavy_check_mark: yes |
| [Using ISO 15118 Plug & Charge with OCPP 1.6](https://www.openchargealliance.org/uploads/files/ocpp_1_6_ISO_15118_v10.pdf)                | :heavy_check_mark: yes                    |
| [Autocharge](https://github.com/openfastchargingalliance/openfastchargingalliance/blob/master/autocharge-final.pdf)                       | :heavy_check_mark: yes |

### Support for OCPP 2.0.1

The development of OCPP2.0.1 has started this year and is in progress. The goal is to provide basic support for OCPP2.0.1 until Q2/2023 and to provide a fully featured implementation by the end of 2023.
## CSMS Compatibility

### CSMS Compatibility OCPP 1.6

The following table shows CSMS with which this library was tested. If you provide a CSMS that is not yet listed here, feel free to [contact us](https://lists.lfenergy.org/g/everest)!

-   chargecloud
-   chargeIQ
-   Chargetic
-   Compleo
-   Current
-   Daimler Truck
-   ev.energy
-   eDRV
-   Fastned
-   GraphDefined
-   Electrip Global
-   EV-Meter
-   Green Motion
-   gridundco
-   ihomer
-   iLumen
-   P3
-   Siemens
-   SteVe
-   Syntech
-   Trialog
-   ubitricity
-   Weev Energy

### CSMS Compatibility OCPP 2.0.1

The current, basic implementation of OCPP 2.0.1 has been tested against a few CSMS and is continously tested against the OCPP Compliance Test Tool 2 (OCTT2) during the implementation.  

## Integration with EVerest

This library is automatically integrated as the OCPP and OCPP201 module within [everest-core](https://github.com/EVerest/everest-core) - the complete software stack for your charging station. It is recommended to use EVerest together with this OCPP implementation.

If you run libocpp with EVerest, the build process of [everest-core](https://github.com/EVerest/everest-core) will take care of installing all necessary dependencies for you.

## Integration with your Charging Station Implementation

OCPP is a protocol that affects, controls and monitors many areas of a charging station's operation.

If you want to integrate this library with your charging station implementation, you have to register a couple of **callbacks** and integrate **event handlers**. This is necessary for the library to interact with your charging station according to the requirements of OCPP.

Libocpp needs registered **callbacks** in order to execute control commands defined within OCPP (e.g Reset.req or RemoteStartTransaction.req)

The implementation must call **event handlers** of libocpp so that the library can track the state of the charging station and trigger OCPP messages accordingly (e.g. MeterValues.req , StatusNotification.req)

Your reference within libocpp to interact is a single instance to the class [ChargePoint](include/ocpp/v16/charge_point.hpp) for OCPP 1.6 or to the class [ChargePoint](include/ocpp/v201/charge_point.hpp) for OCPP 2.0.1.

### Install libocpp

For Debian GNU/Linux 11 you will need the following dependencies:

```bash
  sudo apt install build-essential cmake python3-pip libboost-all-dev libsqlite3-dev libssl-dev
```

Clone this repository.

```bash
  git clone https://github.com/EVerest/libocpp
```

In the libocpp folder create a folder named build and cd into it.
Execute cmake and then make install:

```bash
  mkdir build && cd build
  cmake ..
  make install
```

## Quickstart for OCPP 1.6

Libocpp provides a small standalone OCPP1.6 client that you can control using command line.

Install the dependencies and libocpp as described in [Install libocpp](#install-libocpp).

Make sure you modify the following config entries in the [config.json](config/v16/config.json) file according to the CSMS you want to connect to before executing make install.

```json
{
  "Internal": {
    "ChargePointId": "",
    "CentralSystemURI": ""
  }
}
```

Change into libocpp/build and execute cmake and then make install:

```bash
  cd build
  cmake -DLIBOCPP16_BUILD_EXAMPLES=ON -DCMAKE_INSTALL_PREFIX=./dist ..
  make -j$(nproc) install
```

Use the following command to start the charge point. Replace the config with [config-docker.json](config/v16/config-docker.json) if you want to test with the [SteVe](https://github.com/steve-community/steve#docker) CSMS running in a docker container.

```bash
  ./dist/bin/charge_point \
    --maindir ./dist/share/everest/ocpp/ \
    --conf config.json \
    --logconf ./dist/share/everest/ocpp/logging.ini
```

Type `help` to see a list of possible commands.
