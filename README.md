# C++ implementation of OCPP (currently 1.6 JSON)

This is a C++ library implementation of OCPP, currently in Version 1.6 (https://www.openchargealliance.org/protocols/ocpp-16/) for utilization within the EVerest framework. It enables local chargepoints to communicate with cloud backends for remote management and to allow for payed charging.

The following table shows the current support for the listed feature profiles and application notes. The aim is to fully support all of these in the future.

| Feature-Profile | Supported |
| --- | --- |
| Core | :heavy_check_mark: yes |
| Firmware Management | :x: |
| Local Auth List Management | :x: |
| Reservation | :x: |
| Smart Charging | :yellow_circle: partially |
| Remote Trigger | :heavy_check_mark: yes |

| Application Note | Supported |
| --- | --- |
| OCPP 1.6 Security Whitepaper (3rd edition) | :yellow_circle: partially<br>Supported: Security Profiles 1 & 2 (TLS with HTTP Basic Authentication)<br>No support yet for Profile 3 (TLS with Client Side Certificates)|
| Using ISO 15118 Plug & Charge with OCPP 1.6 | :x: |


All documentation and the issue tracking can be found in our main repository here: https://github.com/EVerest/everest

## Build instructions

Clone the repository:

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

## Launching the charge point

Make sure you modify the following config entries in the aux/config.json file before executing make install:
```json
{
    "Internal": {
        "ChargePointId": "",
        "CentralSystemURI": "",
    }
}
```

Use the following command to start the charge point:

```bash
  cd build
  ./dist/bin/charge_point --maindir ./dist/ocpp --conf config.json
```
