# C++ implementation of OCPP

![Github Actions](https://github.com/EVerest/libocpp/actions/workflows/build_and_test.yaml/badge.svg)

This is a C++ library implementation of OCPP for version 1.6, 2.0.1 and 2.1.
(see [OCPP protocols at OCA website](https://openchargealliance.org/protocols/open-charge-point-protocol/)).
The OCPP2.0.1 implementation of libocpp has been certified by the OCA for multiple hardware platforms.

Libocpp's approach to implementing the  OCPP protocol is to address as much functional requirements as possible as part of the library.
Since OCPP is a protocol that affects, controls, and monitors many areas of a charging station's operation this library needs to be
integrated with your charging station firmware.

## Integration with EVerest

This library is integrated within the [OCPP](https://github.com/EVerest/everest-core/tree/main/modules/OCPP) and [OCPP201](https://github.com/EVerest/everest-core/tree/main/modules/OCPP201)
module within [everest-core](https://github.com/EVerest/everest-core) - the complete software stack for your charging station. It is recommended to use EVerest together with this OCPP implementation.

## Getting Started

Check out the [Getting Started guide](doc/common/getting_started.md). It should be you starting point if you want to integrate this library with your charging station firmware.

## Get Involved

See the [COMMUNITY.md](https://github.com/EVerest/EVerest/blob/main/COMMUNITY.md) and [CONTRIBUTING.md](https://github.com/EVerest/EVerest/blob/main/CONTRIBUTING.md) of the EVerest project to get involved.

## OCPP1.6

### Supported Feature Profiles

OCPP1.6 is fully implemented.

| Feature Profile            | Supported                 |
| -------------------------- | ------------------------- |
| Core                       | ✅ yes    |
| Firmware Management        | ✅ yes    |
| Local Auth List Management | ✅ yes    |
| Reservation                | ✅ yes    |
| Smart Charging             | ✅ yes    |
| Remote Trigger             | ✅ yes    |

| Whitepapers & Application Notes                                                                                                                              | Supported              |
| ----------------------------------------------------------------------------------------------------------------------------------------- | ---------------------- |
| [OCPP 1.6 Security Whitepaper (3rd edition)](https://openchargealliance.org/wp-content/uploads/2023/11/OCPP-1.6-security-whitepaper-edition-3-2.zip) | ✅ yes |
| [Using ISO 15118 Plug & Charge with OCPP 1.6](https://openchargealliance.org/wp-content/uploads/2023/11/ocpp_1_6_ISO_15118_v10.pdf)                | ✅ yes                    |
| [OCPP & California Pricing Requirements](https://openchargealliance.org/wp-content/uploads/2024/09/ocpp_and_dms_evse_regulation-v3.1.pdf)          | ✅ yes |

### CSMS Compatibility

The EVerest implementation of OCPP 1.6 has been tested against the
OCPP Compliance Test Tool (OCTT) during the implementation.

The following table shows the known CSMS with which this library was tested.

- chargecloud
- chargeIQ
- Chargetic
- Compleo
- Current
- Daimler Truck
- ev.energy
- eDRV
- Fastned
- [Open Charging Cloud (GraphDefined)](https://github.com/OpenChargingCloud/WWCP_OCPP)
- Electrip Global
- EnergyStacks
- EV-Meter
- Fraunhofer IAO (ubstack CHARGE)
- Green Motion
- gridundco
- ihomer (Infuse CPMS)
- iLumen
- JibeCompany (CharlieV CMS and Chargebroker proxy)
- MSI
- PUMP (PUMP Connect)
- Scoptvision (Scopt Powerconnect)
- Siemens
- [SteVe](https://github.com/steve-community/steve)
- Syntech
- Trialog
- ubitricity
- Weev Energy

## OCPP2.0.1

### Supported Functional Blocks

| Feature Profile                      | Supported                 |
| -------------------------------------| ------------------------- |
| A. Security                          | ✅ yes  |
| B. Provisioning                      | ✅ yes  |
| C. Authorization                     | ✅ yes  |
| D. LocalAuthorizationList Management | ✅ yes  |
| E. Transactions                      | ✅ yes  |
| F. RemoteControl                     | ✅ yes  |
| G. Availability                      | ✅ yes  |
| H. Reservation                       | ✅ yes                      |
| I. TariffAndCost                     | ✅ yes  |
| J. MeterValues                       | ✅ yes  |
| K. SmartCharging                     | ✅ yes (except K11-K17)                       |
| L. FirmwareManagement                | ✅ yes  |
| M. ISO 15118 CertificateManagement   | ✅ yes  |
| N. Diagnostics                       | ✅ yes  |
| O. DisplayMessage                    | ✅ yes  |
| P. DataTransfer                      | ✅ yes  |

Check the [detailed current implementation status.](doc/v2/ocpp_2x_status.md).

| Whitepapers & Application Notes                                                                                                                              | Supported              |
| ----------------------------------------------------------------------------------------------------------------------------------------- | ---------------------- |
| [OCPP & California Pricing Requirements](https://openchargealliance.org/wp-content/uploads/2024/09/ocpp_and_dms_evse_regulation-v3.1.pdf)          | ✅ yes                  |

### CSMS Compatibility OCPP 2.0.1

The implementation of OCPP 2.0.1 has been tested against the following CSMS and is continuously tested against OCTT2.

Additionally, the implementation has been tested against these CSMS:

- ChargeLab
- Chargepoint
- [CitrineOS](https://lfenergy.org/projects/citrineos/)
- Current
- Ecoenergetyca
- einfochips
- evgateway
- ihomer (Infuse CPMS)
- Instituto Tecnológico de la Energía (ITE)
- [MaEVe (Thoughtworks)](https://github.com/thoughtworks/maeve-csms)
- [Monta](https://monta.com)
- Numocity
- [Open Charging Cloud (GraphDefined)](https://github.com/OpenChargingCloud/WWCP_OCPP)
- Switch EV
- SWTCH
- Relion
- Syntech
- Vector

## OCPP2.1

The implementation of OCPP2.1 is currently under development 🔧.

OCPP2.1 websocket connections and messages are supported by the library. Every functional block and use case supported in OCPP2.0.1 is also supported in OCPP2.1. Additional functional blocks and new requirements are currently
implemented.

The functional blocks we are targeting first are the extensions to SmartCharging and support for Bidirectional Power Transfer.

Check the [detailed current implementation status.](doc/v2/ocpp_2x_status.md).
