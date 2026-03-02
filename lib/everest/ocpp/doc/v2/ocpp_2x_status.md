# OCPP2.0.1 / OCPP2.1 Functional Requirements Status

This document contains the status of which OCPP 2.0.1 and OCPP2.1 numbered functional requirements (FRs) have been implemented in `libocpp`. This does not cover if the functionality is also implemented in `everest-core`.

## Legend

| Status | Description                                                                    |
| ------ | ------------------------------------------------------------------------------ |
| ✅     | Satisfied                                                                      |
| ❎     | Not applicable                                                                 |
| ⛽️     | A functional requirement for other systems in the Charging Station             |
| 🌐     | A functional requirement for the CSMS                                          |
| 💂     | Improper behavior by another actor is guarded against                          |
| ❓     | Actor responsible for or status of requirement is unknown                      |
| 🤓     | Catch-all for FRs that are satisfied for other reasons (see the Remark column) |

## General - General

| ID               | Status | Remark |
| ---------------- | ------ | ------ |
| FR.01            | ✅     |        |
| FR.02            | ✅     |        |
| FR.03            | ✅     |        |
| FR.04            | ❎     |        |
| FR.05            | ✅     |        |
| FR.06 <br> (2.1) |        |        |
| FR.07 <br> (2.1) |        |        |

## Security - Generic Security Profile requirements

| ID         | Status | Remark |
| ---------- | ------ | ------ |
| A00.FR.001 | ✅     |        |
| A00.FR.002 | ✅     |        |
| A00.FR.003 | ✅     |        |
| A00.FR.004 | ✅     |        |
| A00.FR.005 | ✅     |        |
| A00.FR.006 | ✅     |        |

## Security - Unsecured Transport with Basic Authentication Profile

| ID         | Status | Remark |
| ---------- | ------ | ------ |
| A00.FR.201 | ❎     |        |
| A00.FR.202 | ✅     |        |
| A00.FR.203 | ✅     |        |
| A00.FR.204 | ✅     |        |
| A00.FR.205 | ✅     |        |
| A00.FR.206 | ✅     |        |
| A00.FR.207 | ❎     |        |

## Security - TLS with Basic Authentication Profile

| ID         | Status | Remark          |
| ---------- | ------ | --------------- |
| A00.FR.301 | ✅     |                 |
| A00.FR.302 | ✅     |                 |
| A00.FR.303 | ✅     |                 |
| A00.FR.304 | ✅     |                 |
| A00.FR.306 | ❎     |                 |
| A00.FR.307 | ❎     |                 |
| A00.FR.308 | ✅     |                 |
| A00.FR.309 | ✅     |                 |
| A00.FR.310 |        |                 |
| A00.FR.311 | ✅     |                 |
| A00.FR.312 | ✅     |                 |
| A00.FR.313 | ✅     |                 |
| A00.FR.314 | ✅     |                 |
| A00.FR.315 | ❎     |                 |
| A00.FR.316 |        |                 |
| A00.FR.317 | ✅     |                 |
| A00.FR.318 | ❎     |                 |
| A00.FR.319 | ✅     | is configurable |
| A00.FR.320 | ✅     |                 |
| A00.FR.321 | ✅     |                 |
| A00.FR.322 | ❎     |                 |
| A00.FR.323 |        |                 |
| A00.FR.324 | ❎     |                 |

## Security - TLS with Client Side Certificates Profile

| ID                    | Status | Remark |
| --------------------- | ------ | ------ |
| A00.FR.401            | ✅     |        |
| A00.FR.402            | ✅     |        |
| A00.FR.403            | ❎     |        |
| A00.FR.404            | ❎     |        |
| A00.FR.405            | ❎     |        |
| A00.FR.406            | ❎     |        |
| A00.FR.407            | ❎     |        |
| A00.FR.408            | ❎     |        |
| A00.FR.409            | ❎     |        |
| A00.FR.410            | ❎     |        |
| A00.FR.411            | ✅     |        |
| A00.FR.412            | ✅     |        |
| A00.FR.413            |        |        |
| A00.FR.414            | ✅     |        |
| A00.FR.415            | ✅     |        |
| A00.FR.416            | ✅     |        |
| A00.FR.417            | ✅     |        |
| A00.FR.418            | ❎     |        |
| A00.FR.419            |        |        |
| A00.FR.420            | ✅     |        |
| A00.FR.421            | ❎     |        |
| A00.FR.422            | ✅     |        |
| A00.FR.423            | ✅     |        |
| A00.FR.424            | ✅     |        |
| A00.FR.425            | ❎     |        |
| A00.FR.426            |        |        |
| A00.FR.427            | ❎     |        |
| A00.FR.428            | ❎     |        |
| A00.FR.429            | ❎     |        |
| A00.FR.430 <br> (2.1) |        |        |

## Security - Certificate Properties

| ID         | Status | Remark |
| ---------- | ------ | ------ |
| A00.FR.501 | ✅     |        |
| A00.FR.502 | ✅     |        |
| A00.FR.503 | ✅     |        |
| A00.FR.504 | ✅     |        |
| A00.FR.505 | ❎     |        |
| A00.FR.506 | ✅     |        |
| A00.FR.507 | ✅     |        |
| A00.FR.508 | ❎     |        |
| A00.FR.509 | ❎     |        |
| A00.FR.510 | ❎     |        |
| A00.FR.511 | ❎     |        |
| A00.FR.512 | ❎     |        |
| A00.FR.513 | ❎     |        |
| A00.FR.514 | ❎     |        |

## Security - Certificate Hierachy

| ID         | Status | Remark |
| ---------- | ------ | ------ |
| A00.FR.601 | ❎     |        |
| A00.FR.602 | ❎     |        |
| A00.FR.603 | ❎     |        |
| A00.FR.604 | ✅     |        |

## Security - Certificate Revocation

| ID         | Status | Remark |
| ---------- | ------ | ------ |
| A00.FR.701 | ❎     |        |
| A00.FR.702 | ❎     |        |
| A00.FR.703 | ❎     |        |
| A00.FR.704 | ❎     |        |
| A00.FR.705 | ❎     |        |
| A00.FR.707 | ❎     |        |

## Security - Installation

| ID         | Status | Remark |
| ---------- | ------ | ------ |
| A00.FR.801 | ❎     |        |
| A00.FR.802 | ❎     |        |
| A00.FR.803 | ❎     |        |
| A00.FR.804 | ❎     |        |
| A00.FR.805 | ❎     |        |
| A00.FR.806 | ❎     |        |
| A00.FR.807 | ❎     |        |

## Security - Update Charging Station Password for HTTP Basic Authentication

| ID        | Status | Remark |
| --------- | ------ | ------ |
| A01.FR.01 | ✅     |        |
| A01.FR.02 | ✅     |        |
| A01.FR.03 | ❎     |        |
| A01.FR.04 | ❎     |        |
| A01.FR.05 | ❎     |        |
| A01.FR.06 | ❎     |        |
| A01.FR.07 | ❎     |        |
| A01.FR.08 | ❎     |        |
| A01.FR.09 | ❎     |        |
| A01.FR.10 | ✅     |        |
| A01.FR.11 |        |        |
| A01.FR.12 | ✅     |        |

## Security - Update Charging Station Certificate by request of CSMS

| ID                   | Status | Remark                                                           |
| -------------------- | ------ | ---------------------------------------------------------------- |
| A02.FR.01            | ❎     |                                                                  |
| A02.FR.02            | ✅     |                                                                  |
| A02.FR.03            | ✅     |                                                                  |
| A02.FR.04            | ❎     |                                                                  |
| A02.FR.05            | ✅     |                                                                  |
| A02.FR.06            | ✅     |                                                                  |
| A02.FR.07            | ✅     |                                                                  |
| A02.FR.08            |        | This is done on next use of cert if cert is valid in the future. |
| A02.FR.09            | ✅     |                                                                  |
| A02.FR.10            | ❎     |                                                                  |
| A02.FR.11            | ❎     |                                                                  |
| A02.FR.12            | ❎     |                                                                  |
| A02.FR.13            | ✅     |                                                                  |
| A02.FR.14            | ❎     |                                                                  |
| A02.FR.15            | ✅     |                                                                  |
| A02.FR.16            |        |                                                                  |
| A02.FR.17            | ✅     |                                                                  |
| A02.FR.18            | ✅     |                                                                  |
| A02.FR.19            | ✅     |                                                                  |
| A02.FR.19 <br> (2.1) |        |                                                                  |
| A02.FR.20            | ✅     |                                                                  |
| A02.FR.20 <br> (2.1) |        |                                                                  |
| A02.FR.21            |        |                                                                  |
| A02.FR.22 <br> (2.1) |        |                                                                  |
| A02.FR.23 <br> (2.1) |        |                                                                  |
| A02.FR.24 <br> (2.1) |        |                                                                  |
| A02.FR.25 <br> (2.1) |        |                                                                  |
| A02.FR.26 <br> (2.1) |        |                                                                  |
| A02.FR.27 <br> (2.1) |        |                                                                  |
| A02.FR.28 <br> (2.1) |        |                                                                  |
| A02.FR.29 <br> (2.1) |        |                                                                  |

## Security - Update Charging Station Certificate initiated by the Charging Station

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| A03.FR.01            | ❎     |        |
| A03.FR.02            | ✅     |        |
| A03.FR.03            | ✅     |        |
| A03.FR.04            | ❎     |        |
| A03.FR.05            | ✅     |        |
| A03.FR.06            | ✅     |        |
| A03.FR.07            | ✅     |        |
| A03.FR.08            |        |        |
| A03.FR.09            | ✅     |        |
| A03.FR.10            | ❎     |        |
| A03.FR.11            | ❎     |        |
| A03.FR.12            | ❎     |        |
| A03.FR.13            | ✅     |        |
| A03.FR.14            | ❎     |        |
| A03.FR.15            | ✅     |        |
| A03.FR.16            |        |        |
| A03.FR.17            | ✅     |        |
| A03.FR.18            | ✅     |        |
| A03.FR.19            | ✅     |        |
| A03.FR.19 <br> (2.1) |        |        |
| A03.FR.20 <br> (2.1) |        |        |
| A03.FR.21 <br> (2.1) |        |        |
| A03.FR.22 <br> (2.1) |        |        |
| A03.FR.23 <br> (2.1) |        |        |
| A03.FR.24 <br> (2.1) |        |        |
| A03.FR.25 <br> (2.1) |        |        |

## Security - Security Event Notification

| ID        | Status | Remark |
| --------- | ------ | ------ |
| A04.FR.01 | ✅     |        |
| A04.FR.02 | ✅     |        |
| A04.FR.03 | ❎     |        |
| A04.FR.04 | ✅     |        |

## Security - Upgrade Charging Station Security Profile

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| A05.FR.02            | ✅     |        |
| A05.FR.03            | ✅     |        |
| A05.FR.04            | ✅     |        |
| A05.FR.05            | ✅     |        |
| A05.FR.06            |        |        |
| A05.FR.07            | ❎     |        |
| A05.FR.08 <br> (2.1) |        |        |
| A05.FR.09 <br> (2.1) |        |        |
| A05.FR.10 <br> (2.1) |        |        |

## Provisioning - Cold Boot Charging Station

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| B01.FR.01            | ✅     |        |
| B01.FR.02            | ❎     |        |
| B01.FR.03            | ✅     |        |
| B01.FR.04            | ✅     |        |
| B01.FR.05            | ✅     |        |
| B01.FR.05 <br> (2.1) |        |        |
| B01.FR.06            | ❎     |        |
| B01.FR.07            | ✅     |        |
| B01.FR.08            | ✅     |        |
| B01.FR.09            | ✅     |        |
| B01.FR.10            | ❎     |        |
| B01.FR.11            | ❎     |        |
| B01.FR.12            | ❎     |        |
| B01.FR.13            | ✅     |        |

## Provisioning - Cold Boot Charging Station – Pending

| ID        | Status | Remark                                           |
| --------- | ------ | ------------------------------------------------ |
| B02.FR.01 | ✅     |                                                  |
| B02.FR.02 | ✅     | To be tested manually (probably alrady has been) |
| B02.FR.03 | ✅     |                                                  |
| B02.FR.04 | ✅     |                                                  |
| B02.FR.05 | ✅     |                                                  |
| B02.FR.06 | ✅     |                                                  |
| B02.FR.07 | ✅     |                                                  |
| B02.FR.08 | ✅     |                                                  |
| B02.FR.09 | ❎     |                                                  |

## Provisioning - Cold Boot Charging Station – Rejected

| ID        | Status | Remark |
| --------- | ------ | ------ |
| B03.FR.01 | ✅     |        |
| B03.FR.02 | ✅     |        |
| B03.FR.03 | ❎     |        |
| B03.FR.04 | ✅     |        |
| B03.FR.05 | ✅     |        |
| B03.FR.06 | ✅     |        |
| B03.FR.07 | ❎     |        |
| B03.FR.08 | ✅     |        |

## Provisioning - Offline Behavior Idle Charging Station

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| B04.FR.01            | ✅     |        |
| B04.FR.02            | ✅     |        |
| B04.FR.01 <br> (2.1) |        |        |
| B04.FR.02 <br> (2.1) |        |        |

## Provisioning - Set Variables

| ID        | Status | Remark |
| --------- | ------ | ------ |
| B05.FR.01 | ✅     |        |
| B05.FR.02 | ✅     |        |
| B05.FR.03 | ✅     |        |
| B05.FR.04 | ✅     |        |
| B05.FR.05 | ✅     |        |
| B05.FR.06 | ✅     |        |
| B05.FR.07 | ✅     |        |
| B05.FR.08 | ✅     |        |
| B05.FR.09 | ✅     |        |
| B05.FR.10 | ✅     |        |
| B05.FR.11 | ❎     |        |
| B05.FR.12 | ✅     |        |
| B05.FR.13 | ✅     |        |

## Provisioning - Get Variables

| ID        | Status | Remark |
| --------- | ------ | ------ |
| B06.FR.01 | ✅     |        |
| B06.FR.02 | ✅     |        |
| B06.FR.03 | ✅     |        |
| B06.FR.04 | ✅     |        |
| B06.FR.05 | ✅     |        |
| B06.FR.06 | ✅     |        |
| B06.FR.07 | ✅     |        |
| B06.FR.08 | ✅     |        |
| B06.FR.09 | ✅     |        |
| B06.FR.10 | ✅     |        |
| B06.FR.11 | ✅     |        |
| B06.FR.13 | ✅     |        |
| B06.FR.14 | ✅     |        |
| B06.FR.15 | ✅     |        |
| B06.FR.16 | ✅     |        |
| B06.FR.17 | ✅     |        |

## Provisioning - Get Base Report

| ID                   | Status | Remark                    |
| -------------------- | ------ | ------------------------- |
| B07.FR.01            | ✅     |                           |
| B07.FR.02            | ✅     |                           |
| B07.FR.03            | ✅     |                           |
| B07.FR.04            | ✅     |                           |
| B07.FR.05            | ✅     |                           |
| B07.FR.06            | ✅     |                           |
| B07.FR.07            | ✅     |                           |
| B07.FR.08            | ✅     |                           |
| B07.FR.09            | ✅     |                           |
| B07.FR.10            | ✅     |                           |
| B07.FR.11            | ✅     |                           |
| B07.FR.12            | ✅     |                           |
| B07.FR.13            | ❎     | tbd if this is applicable |
| B07.FR.14            | ❎     |                           |
| B07.FR.15 <br> (2.1) |        |                           |

## Provisioning - Get Custom Report

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| B08.FR.01            | ✅     |        |
| B08.FR.02            | ✅     |        |
| B08.FR.03            | ✅     |        |
| B08.FR.04            | ✅     |        |
| B08.FR.05            | ✅     |        |
| B08.FR.06            | ❎     |        |
| B08.FR.07            | ✅     |        |
| B08.FR.08            | ✅     |        |
| B08.FR.09            | ✅     |        |
| B08.FR.10            | ✅     |        |
| B08.FR.11            | ✅     |        |
| B08.FR.12            | ✅     |        |
| B08.FR.13            | ✅     |        |
| B08.FR.14            | ✅     |        |
| B08.FR.15            | ✅     |        |
| B08.FR.16            | ✅     |        |
| B08.FR.17            | ✅     |        |
| B08.FR.18            | ✅     |        |
| B08.FR.19            |        |        |
| B08.FR.20            |        |        |
| B08.FR.21            |        |        |
| B08.FR.22 <br> (2.1) |        |        |
| B08.FR.23 <br> (2.1) |        |        |
| B08.FR.24 <br> (2.1) |        |        |
| B08.FR.25 <br> (2.1) |        |        |

## Provisioning - Setting a new NetworkConnectionProfile

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| B09.FR.01            | ✅     |        |
| B09.FR.02            | ✅     |        |
| B09.FR.03            | ✅     |        |
| B09.FR.04            | ✅     |        |
| B09.FR.05            |        |        |
| B09.FR.06            |        |        |
| B09.FR.07 <br> (2.1) |        |        |
| B09.FR.08 <br> (2.1) |        |        |
| B09.FR.09 <br> (2.1) |        |        |
| B09.FR.10 <br> (2.1) |        |        |
| B09.FR.11 <br> (2.1) |        |        |
| B09.FR.12 <br> (2.1) |        |        |
| B09.FR.13 <br> (2.1) |        |        |
| B09.FR.14 <br> (2.1) |        |        |
| B09.FR.15 <br> (2.1) |        |        |
| B09.FR.16 <br> (2.1) |        |        |
| B09.FR.17 <br> (2.1) |        |        |
| B09.FR.18 <br> (2.1) |        |        |
| B09.FR.19 <br> (2.1) |        |        |
| B09.FR.20 <br> (2.1) |        |        |
| B09.FR.21 <br> (2.1) |        |        |
| B09.FR.22 <br> (2.1) |        |        |
| B09.FR.23 <br> (2.1) |        |        |
| B09.FR.24 <br> (2.1) |        |        |
| B09.FR.25 <br> (2.1) |        |        |
| B09.FR.26 <br> (2.1) |        |        |
| B09.FR.27 <br> (2.1) |        |        |
| B09.FR.28 <br> (2.1) |        |        |
| B09.FR.29 <br> (2.1) |        |        |
| B09.FR.30 <br> (2.1) |        |        |
| B09.FR.31            |        |        |
| B09.FR.32            |        |        |

## Provisioning - Migrate to new CSMS

| ID                   | Status | Remark                                                      |
| -------------------- | ------ | ----------------------------------------------------------- |
| B10.FR.01            | ✅     |                                                             |
| B10.FR.02            | ✅     |                                                             |
| B10.FR.03            | ✅     |                                                             |
| B10.FR.04            | ✅     |                                                             |
| B10.FR.05            |        |                                                             |
| B10.FR.06            | ✅     |                                                             |
| B10.FR.07            | ✅     | tbd. we're looping over priorities and attempt to reconnect |
| B10.FR.08 <br> (2.1) |        |                                                             |
| B10.FR.09 <br> (2.1) |        |                                                             |

## Provisioning - Reset - Without Ongoing Transaction

| ID        | Status | Remark                                        |
| --------- | ------ | --------------------------------------------- |
| B11.FR.01 | ✅     |                                               |
| B11.FR.02 | ✅     |                                               |
| B11.FR.03 | ✅     |                                               |
| B11.FR.04 | ✅     |                                               |
| B11.FR.05 | ✅     |                                               |
| B11.FR.06 | ⛽️    | In EVerest, the System module is responsible. |
| B11.FR.07 | ⛽️    | In EVerest, the System module is responsible. |
| B11.FR.08 | ✅     |                                               |
| B11.FR.09 | ✅     |                                               |
| B11.FR.10 | ✅     | has to be set in device model                 |

## Provisioning - Reset - With Ongoing Transaction

| ID        | Status | Remark                                                                           |
| --------- | ------ | -------------------------------------------------------------------------------- |
| B12.FR.01 | ✅     |                                                                                  |
| B12.FR.02 | ✅     |                                                                                  |
| B12.FR.03 | ✅     |                                                                                  |
| B12.FR.04 | ✅     |                                                                                  |
| B12.FR.05 | ✅     |                                                                                  |
| B12.FR.06 | ⛽️    | Charging station is responsible to send the correct state after booting          |
| B12.FR.07 | ✅     |                                                                                  |
| B12.FR.08 | ✅     |                                                                                  |
| B12.FR.09 | ⛽️    | Charging Station should respond with a "rejected" on `is_reset_allowed_callback` |
| B12.FR.10 |        |                                                                                  |

## Provisioning - Reset - With Ongoing Transaction - Without Termination (New in OCPP 2.1)

| ID        | Status                    | Remark |
| --------- | ------------------------- | ------ |
| B13.FR.01 |                           |        |
| B13.FR.02 |                           |        |
| B13.FR.03 |                           |        |
| B13.FR.04 |                           |        |
| B13.FR.05 |                           |        |
| B13.FR.06 |                           |        |
| B13.FR.07 |                           |        |
| B13.FR.08 |                           |        |
| B13.FR.09 |                           |        |
| B13.FR.10 |                           |        |
| B13.FR.11 |                           |        |
| B13.FR.12 |                           |        |
| B13.FR.13 |                           |        |
| B13.FR.14 |                           |        |
|           | Reset of Charging Station |        |
| B13.FR.20 |                           |        |
| B13.FR.21 |                           |        |
| B13.FR.22 |                           |        |
| B13.FR.23 |                           |        |
| B13.FR.24 |                           |        |
| B13.FR.25 |                           |        |
|           | Signal resumption         |        |
| B13.FR.30 |                           |        |
| B13.FR.31 |                           |        |

## Authorization - EV Driver Authorization using RFID

| ID                   | Status | Remark                                           |
| -------------------- | ------ | ------------------------------------------------ |
| C01.FR.01            | ✅     |                                                  |
| C01.FR.02            | ✅     |                                                  |
| C01.FR.03            | ✅     |                                                  |
| C01.FR.04            | ✅     |                                                  |
| C01.FR.05            | ✅     |                                                  |
| C01.FR.06            | ✅     |                                                  |
| C01.FR.07            | ✅     |                                                  |
| C01.FR.08            |        | This to FR.17 are all language related usecases. |
| C01.FR.09            |        |                                                  |
| C01.FR.10            |        |                                                  |
| C01.FR.11            |        |                                                  |
| C01.FR.12            |        |                                                  |
| C01.FR.13            |        |                                                  |
| C01.FR.17            |        |                                                  |
| C01.FR.18            | ✅     |                                                  |
| C01.FR.19            | ✅     |                                                  |
| C01.FR.20            | ✅     |                                                  |
| C01.FR.21            | ✅     | Auth mechanism is responsible.                   |
| C01.FR.22            | ✅     |                                                  |
| C01.FR.23            | ✅     |                                                  |
| C01.FR.24            | ✅     |                                                  |
| C01.FR.25 <br> (2.1) |        |                                                  |
| C01.FR.26 <br> (2.1) |        |                                                  |

## Authorization - Authorization using a start button

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C02.FR.01 | ❎     |        |
| C02.FR.02 | 🌐     |        |
| C02.FR.03 | ✅     |        |

## Authorization - Authorization using credit/debit card

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C03.FR.01 | ✅     |        |
| C03.FR.02 | ✅     |        |

## Authorization - Authorization using PIN-code

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C04.FR.01 | ❎     |        |
| C04.FR.02 | ❎     |        |
| C04.FR.03 | ❎     |        |
| C04.FR.04 | ❎     |        |
| C04.FR.05 | ❎     |        |
| C04.FR.06 | ❎     |        |

## Authorization - Authorization for CSMS initiated transactions

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C05.FR.01 | ✅     |        |
| C05.FR.02 | ✅     |        |
| C05.FR.03 | ⛽️    |        |
| C05.FR.04 |        |        |
| C05.FR.05 | ✅     |        |

## Authorization - Authorization using local id type

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C06.FR.01 | ✅     |        |
| C06.FR.02 | ✅     |        |
| C06.FR.03 | ✅     |        |
| C06.FR.04 | ❎     |        |

## Authorization - Authorization using Contract Certificates

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C07.FR.01 | ✅     |        |
| C07.FR.02 | ✅     |        |
| C07.FR.04 | ❎     |        |
| C07.FR.05 | ❎     |        |
| C07.FR.06 | ✅     |        |
| C07.FR.07 | ✅     |        |
| C07.FR.08 | ✅     |        |
| C07.FR.09 | ✅     |        |
| C07.FR.10 | ✅     |        |
| C07.FR.11 | ✅     |        |
| C07.FR.12 | ✅     |        |
| C07.FR.13 | 🌐     |        |
| C07.FR.14 | 🌐     |        |
| C07.FR.15 | 🌐     |        |
| C07.FR.16 | 🌐     |        |
| C07.FR.17 | 🌐     |        |

## Authorization - Authorization at EVSE using ISO 15118 External Identification Means (EIM)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C08.FR.01 |        |        |
| C08.FR.02 |        |        |

## Authorization - Authorization by GroupId

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| C09.FR.02            | ❎     |        |
| C09.FR.03            | ✅     |        |
| C09.FR.04            | ✅     |        |
| C09.FR.05            | ✅     |        |
| C09.FR.07            | ✅     |        |
| C09.FR.09            | 🌐     |        |
| C09.FR.10            | 🌐     |        |
| C09.FR.11            | ✅     |        |
| C09.FR.12            | 🌐     |        |
| C09.FR.13 <br> (2.1) | 🌐     |        |

## Authorization - Store Authorization Data in the Authorization Cache

| ID                   | Status | Remark      |
| -------------------- | ------ | ----------- |
| C10.FR.01            | ✅     |             |
| C10.FR.02            | ✅     |             |
| C10.FR.03            | ✅     |             |
| C10.FR.04            | ✅     |             |
| C10.FR.05            | ✅     |             |
| C10.FR.06            |        | Reservation |
| C10.FR.07            | ✅     | deferred    |
| C10.FR.08            | ✅     |             |
| C10.FR.09            |        | deferred    |
| C10.FR.10            | ✅     |             |
| C10.FR.11            | ✅     |             |
| C10.FR.12            | ✅     |             |
| C10.FR.13            |        |             |
| C10.FR.14 <br> (2.1) |        |             |

## Authorization - Clear Authorization Data in Authorization Cache

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C11.FR.01 | ✅     |        |
| C11.FR.02 | ✅     |        |
| C11.FR.03 | ✅     |        |
| C11.FR.04 | ✅     |        |
| C11.FR.05 | ✅     |        |

## Authorization - Start Transaction - Cached Id

| ID        | Status | Remark                                      |
| --------- | ------ | ------------------------------------------- |
| C12.FR.02 | ✅     |                                             |
| C12.FR.03 | ✅     |                                             |
| C12.FR.04 | ✅     |                                             |
| C12.FR.05 | ✅     |                                             |
| C12.FR.06 | ✅     |                                             |
| C12.FR.09 | ⛽️    | In EVerest, the Auth module is responsible. |

## Authorization - Offline Authorization through Local Authorization List

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C13.FR.01 | ✅     |        |
| C13.FR.02 | ✅     |        |
| C13.FR.03 | ✅     |        |
| C13.FR.04 | ✅     |        |
| C13.FR.05 |        |        |
| C13.FR.06 |        |        |

## Authorization - Online Authorization through Local Authorization List

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C14.FR.01 | ✅     |        |
| C14.FR.02 | ✅     |        |
| C14.FR.03 | ✅     |        |
| C14.FR.04 |        |        |
| C14.FR.05 |        |        |

## Authorization - Offline Authorization of unknown Id

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C15.FR.01 | ✅     |        |
| C15.FR.02 | ✅     |        |
| C15.FR.03 | ✅     |        |
| C15.FR.04 | ✅     |        |
| C15.FR.05 | ⛽️    |        |
| C15.FR.06 | ✅     |        |
| C15.FR.07 | ✅     |        |
| C15.FR.08 | ✅     |        |

## Authorization - Stop Transaction with a Master Pass

| ID                   | Status | Remark        |
| -------------------- | ------ | ------------- |
| C16.FR.01            |        |               |
| C16.FR.02            | ⛽️    | Core changes? |
| C16.FR.03            | ⛽️    | Core changes  |
| C16.FR.04            |        |               |
| C16.FR.05            |        |               |
| C16.FR.07 <br> (2.1) |        |               |
| C16.FR.08 <br> (2.1) |        |               |

## Authorization - EV Driver Authorization using prepaid card (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C17.FR.01 |        |        |
| C17.FR.02 |        |        |
| C17.FR.03 |        |        |
| C17.FR.04 |        |        |
| C17.FR.05 |        |        |

## Authorization - Authorization using locally connected payment terminal (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C18.FR.01 |        |        |
| C18.FR.02 |        |        |
| C18.FR.03 |        |        |
| C18.FR.04 |        |        |
| C18.FR.05 |        |        |
| C18.FR.06 |        |        |
| C18.FR.07 |        |        |
| C18.FR.08 |        |        |
| C18.FR.09 |        |        |
| C18.FR.10 |        |        |

## Authorization - Cancelation prior to transaction (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C19.FR.01 |        |        |
| C19.FR.02 |        |        |

## Authorization - Cancelation after start of transaction (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C20.FR.01 |        |        |
| C20.FR.02 |        |        |
| C20.FR.03 |        |        |
| C20.FR.04 |        |        |
| C20.FR.05 |        |        |

## Authorization - Settlement at end of transaction (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C21.FR.01 |        |        |
| C21.FR.02 |        |        |
| C21.FR.03 |        |        |
| C21.FR.04 |        |        |
| C21.FR.05 |        |        |
| C21.FR.06 |        |        |
| C21.FR.07 |        |        |

## Authorization - Settlement is rejected or fails (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C22.FR.01 |        |        |
| C22.FR.02 |        |        |

## Authorization - Incremental authorization (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| C23.FR.01 |        |        |
| C23.FR.02 |        |        |

## Authorization - Ad hoc payment via stand-alone payment terminal (New in OCPP 2.1)

| ID        | Status     | Remark |
| --------- | ---------- | ------ |
| C24.FR.01 |            |        |
| C24.FR.02 |            |        |
| C24.FR.03 |            |        |
| C24.FR.04 |            |        |
|           | Settlement |        |
| C24.FR.10 |            |        |
| C24.FR.11 |            |        |

## Authorization - Ad hoc payment via static or dynamic QR code (New in OCPP 2.1)

| ID        | Status                               | Remark |
| --------- | ------------------------------------ | ------ |
|           | URL validation for dynamic QR code   |        |
| C25.FR.01 |                                      |        |
| C25.FR.03 |                                      |        |
| C25.FR.04 |                                      |        |
| C25.FR.05 |                                      |        |
| C25.FR.06 |                                      |        |
| C25.FR.07 |                                      |        |
| C25.FR.08 |                                      |        |
| C25.FR.09 |                                      |        |
|           | Payment authorization                |        |
| C25.FR.20 |                                      |        |
| C25.FR.21 |                                      |        |
| C25.FR.22 |                                      |        |
| C25.FR.23 |                                      |        |
| C25.FR.24 |                                      |        |
| C25.FR.25 |                                      |        |
| C25.FR.26 |                                      |        |
| C25.FR.27 |                                      |        |
|           | Settlement                           |        |
| C25.FR.40 |                                      |        |
| C25.FR.41 |                                      |        |
|           | URL requirements for dynamic QR code |        |
| C25.FR.50 |                                      |        |
| C25.FR.51 |                                      |        |
| C25.FR.52 |                                      |        |
| C25.FR.53 |                                      |        |
| C25.FR.56 |                                      |        |
| C25.FR.57 |                                      |        |
| C25.FR.58 |                                      |        |
| C25.FR.59 |                                      |        |
| C25.FR.60 |                                      |        |

## LocalAuthorizationListManagement - Send Local Authorization List

| ID        | Status | Remark |
| --------- | ------ | ------ |
| D01.FR.01 | ✅     |        |
| D01.FR.02 | ✅     |        |
| D01.FR.03 | 🌐     |        |
| D01.FR.04 | ✅     |        |
| D01.FR.05 | ✅     |        |
| D01.FR.06 | ✅     |        |
| D01.FR.09 | ✅     |        |
| D01.FR.10 | ✅     |        |
| D01.FR.11 | ✅     |        |
| D01.FR.12 | ✅     |        |
| D01.FR.13 | ✅     |        |
| D01.FR.15 | ✅     |        |
| D01.FR.16 | ✅     |        |
| D01.FR.17 | ✅     |        |
| D01.FR.18 | ✅     |        |
| D01.FR.19 | ✅     |        |

## LocalAuthorizationListManagement - Get Local List Version

| ID        | Status | Remark |
| --------- | ------ | ------ |
| D02.FR.01 | ✅     |        |
| D02.FR.02 | ✅     |        |
| D02.FR.03 | ✅     |        |

## Transactions - Start Transaction Options

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E01.FR.01 | ❎     |        |
| E01.FR.02 | ❎     |        |
| E01.FR.03 | ❎     |        |
| E01.FR.04 | ❎     |        |
| E01.FR.05 | ✅     |        |
| E01.FR.06 | ❎     |        |
| E01.FR.07 | ✅     |        |
| E01.FR.08 | ✅     |        |
| E01.FR.09 | ✅     |        |
| E01.FR.10 | ✅     |        |
| E01.FR.11 | ❎     |        |
| E01.FR.12 | ❎     |        |
| E01.FR.13 |        |        |
| E01.FR.14 | ✅     |        |
| E01.FR.15 | ✅     |        |
| E01.FR.16 | ✅     |        |
| E01.FR.17 | ❎     |        |
| E01.FR.18 | ✅     |        |
| E01.FR.19 | ✅     |        |
| E01.FR.20 | ❎     | tbd    |

## Transactions - Start Transaction - Cable Plugin First

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| E02.FR.01            | ✅     |        |
| E02.FR.02            | ✅     |        |
| E02.FR.03            |        |        |
| E02.FR.04            | ✅     |        |
| E02.FR.05 <br> (2.1) |        |        |
| E02.FR.06            | ❎     |        |
| E02.FR.07            | ✅     |        |
| E02.FR.08            | ✅     |        |
| E02.FR.09            | ✅     |        |
| E02.FR.10            | ✅     |        |
| E02.FR.11            | ❎     | tbd    |
| E02.FR.13            | ✅     |        |
| E02.FR.14            | ✅     |        |
| E02.FR.15            | ✅     |        |
| E02.FR.16            | ✅     |        |
| E02.FR.17            | ✅     |        |
| E02.FR.18            |        |        |
| E02.FR.19            |        |        |
| E02.FR.20            | ✅     |        |
| E02.FR.21            | ✅     |        |

## Transactions - Start Transaction - IdToken First

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E03.FR.01 | ✅     |        |
| E03.FR.02 | ✅     |        |
| E03.FR.03 |        |        |
| E03.FR.05 | ❎     |        |
| E03.FR.06 | ✅     |        |
| E03.FR.07 | ✅     |        |
| E03.FR.08 | ✅     |        |
| E03.FR.09 | ❎     | tbd    |
| E03.FR.10 | ✅     |        |
| E03.FR.11 | ✅     |        |
| E03.FR.12 | ✅     |        |
| E03.FR.13 |        |        |
| E03.FR.14 |        |        |
| E03.FR.15 |        |        |

## Transactions - Transaction started while Charging Station is offline

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E04.FR.01 | ✅     |        |
| E04.FR.02 | ✅     |        |
| E04.FR.03 | ✅     |        |
| E04.FR.04 | ✅     |        |
| E04.FR.05 | ✅     |        |
| E04.FR.06 | ✅     |        |
| E04.FR.07 |        | tbd    |
| E04.FR.08 |        | tbd    |
| E04.FR.09 |        | tbd    |
| E04.FR.10 | ✅     | tbd    |
| E04.FR.11 |        |        |

## Transactions - Start Transaction - Id not Accepted

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| E05.FR.01            | ✅     |        |
| E05.FR.02            | ✅     |        |
| E05.FR.03            | ✅     |        |
| E05.FR.04            | ✅     |        |
| E05.FR.05            | ✅     |        |
| E05.FR.06            | ✅     |        |
| E05.FR.08            | ✅     |        |
| E05.FR.09 <br> (2.1) |        |        |
| E05.FR.10            | ✅     |        |
| E05.FR.11            | ❎     |        |

## Transactions - Stop Transaction options

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E06.FR.01 | ❎     |        |
| E06.FR.02 | ✅     |        |
| E06.FR.03 | ✅     |        |
| E06.FR.04 | ✅     |        |
| E06.FR.05 | ❎     |        |
| E06.FR.06 | ❎     |        |
| E06.FR.07 | ❎     |        |
| E06.FR.08 | ✅     |        |
| E06.FR.09 | ✅     |        |
| E06.FR.10 | ❎     |        |
| E06.FR.11 | ✅     |        |
| E06.FR.12 | ❎     | tbd    |
| E06.FR.13 | ❎     | tbd    |
| E06.FR.14 | ✅     |        |
| E06.FR.15 | ✅     |        |
| E06.FR.16 |        |        |

## Transactions - Transaction locally stopped by IdToken

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E07.FR.01 | ✅     |        |
| E07.FR.02 | ✅     |        |
| E07.FR.04 | ✅     |        |
| E07.FR.05 | ✅     |        |
| E07.FR.06 | ✅     |        |
| E07.FR.07 | ❎     |        |
| E07.FR.08 | ✅     |        |
| E07.FR.09 | ✅     |        |
| E07.FR.10 | ✅     |        |
| E07.FR.11 | ✅     |        |
| E07.FR.12 | ✅     |        |

## Transactions - Transaction stopped while Charging Station is offline

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E08.FR.01 | ✅     |        |
| E08.FR.02 | ✅     |        |
| E08.FR.03 | ❎     |        |
| E08.FR.04 | ✅     |        |
| E08.FR.05 | ✅     |        |
| E08.FR.06 | ✅     |        |
| E08.FR.07 | ✅     |        |
| E08.FR.08 | ✅     |        |
| E08.FR.09 | ✅     |        |
| E08.FR.10 | ✅     |        |
| E08.FR.11 | ✅     |        |
| E08.FR.12 | ✅     |        |

## Transactions - When cable disconnected on EV-side: Stop Transaction

| ID                   | Status | Remark                                                         |
| -------------------- | ------ | -------------------------------------------------------------- |
| E09.FR.01            | ✅     | `StopTxOnEVSideDisconnect` is RO for our implementation so far |
| E09.FR.02            |        |                                                                |
| E09.FR.03            |        |                                                                |
| E09.FR.04            | ✅     |                                                                |
| E09.FR.05            | ✅     |                                                                |
| E09.FR.06            | ✅     |                                                                |
| E09.FR.07            | ✅     |                                                                |
| E09.FR.08 <br> (2.1) |        |                                                                |

## Transactions - When cable disconnected on EV-side: Suspend Transaction

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E10.FR.01 |        |        |
| E10.FR.02 | ✅     |        |
| E10.FR.03 | ✅     |        |
| E10.FR.04 | ✅     |        |
| E10.FR.05 | ❎     | tbd    |
| E10.FR.06 |        | tbd    |
| E10.FR.07 | ✅     | tbd    |

## Transactions - Connection Loss During Transaction

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E11.FR.01 | ✅     |        |
| E11.FR.02 | ✅     |        |
| E11.FR.03 | ✅     |        |
| E11.FR.04 | ✅     |        |
| E11.FR.05 | ✅     |        |
| E11.FR.06 | ✅     |        |
| E11.FR.07 | ✅     |        |
| E11.FR.08 | ✅     |        |

## Transactions - Inform CSMS of an Offline Occurred Transaction

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| E12.FR.01            | ✅     |        |
| E12.FR.02            | ✅     |        |
| E12.FR.03            | ✅     |        |
| E12.FR.04 <br> (2.1) | ✅     |        |
| E12.FR.05            | ✅     |        |
| E12.FR.06            | ✅     |        |
| E12.FR.07            | ✅     |        |
| E12.FR.08            | ✅     |        |
| E12.FR.09            | ✅     |        |
| E12.FR.10            | ✅     |        |

## Transactions - Transaction-related message not accepted by CSMS

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E13.FR.01 | ✅     |        |
| E13.FR.02 | ✅     |        |
| E13.FR.03 | ✅     |        |
| E13.FR.04 | ✅     |        |

## Transactions - Check transaction status

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E14.FR.01 | ✅     |        |
| E14.FR.02 | ✅     |        |
| E14.FR.03 | ✅     |        |
| E14.FR.04 | ✅     |        |
| E14.FR.05 | ✅     |        |
| E14.FR.06 | ✅     |        |
| E14.FR.07 | ✅     |        |
| E14.FR.08 | ✅     |        |

## Transactions - End of charging process

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| E15.FR.01            | ✅     |        |
| E15.FR.02            | ❎     | tbd    |
| E15.FR.03            | ❎     | tbd    |
| E15.FR.04            | ✅     |        |
| E15.FR.05 <br> (2.1) |        |        |

## Transactions - Transactions with fixed cost, energy, SoC or time (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| E16.FR.01 |        |        |
| E16.FR.02 |        |        |
| E16.FR.03 |        |        |
| E16.FR.04 |        |        |
| E16.FR.05 |        |        |
| E16.FR.06 |        |        |
| E16.FR.07 |        |        |
| E16.FR.08 |        |        |
| E16.FR.09 |        |        |
| E16.FR.10 |        |        |
| E16.FR.11 |        |        |
| E16.FR.12 |        |        |
| E16.FR.13 |        |        |
| E16.FR.14 |        |        |
| E16.FR.15 |        |        |
| E16.FR.16 |        |        |
| E16.FR.17 |        |        |
| E16.FR.18 |        |        |
| E16.FR.19 |        |        |
| E16.FR.20 |        |        |

## Transactions - Resuming transaction after interruption (New in OCPP 2.1)

| ID        | Status         | Remark |
| --------- | -------------- | ------ |
| E17.FR.01 |                |        |
| E17.FR.02 |                |        |
|           | Within timeout |        |
| E17.FR.10 |                |        |
| E17.FR.11 |                |        |
| E17.FR.12 |                |        |
| E17.FR.13 |                |        |
| E17.FR.14 |                |        |
| E17.FR.15 |                |        |
| E17.FR.16 |                |        |
| E17.FR.17 |                |        |
| E17.FR.18 |                |        |
|           | After timeout  |        |
| E17.FR.20 |                |        |
| E17.FR.21 |                |        |
| E17.FR.22 |                |        |

## RemoteControl - Remote Start Transaction - Cable Plugin First

| ID        | Status | Remark                                                   |
| --------- | ------ | -------------------------------------------------------- |
| F01.FR.01 | ⛽️    |                                                          |
| F01.FR.02 | ⛽️    |                                                          |
| F01.FR.03 | ⛽️    |                                                          |
| F01.FR.04 | ⛽️    |                                                          |
| F01.FR.05 | ⛽️    |                                                          |
| F01.FR.06 | ✅     |                                                          |
| F01.FR.07 | ✅     | Currently always rejected                                |
| F01.FR.08 |        |                                                          |
| F01.FR.09 |        |                                                          |
| F01.FR.10 |        |                                                          |
| F01.FR.11 |        |                                                          |
| F01.FR.12 |        |                                                          |
| F01.FR.13 | ⛽️    |                                                          |
| F01.FR.14 | ⛽️    |                                                          |
| F01.FR.15 | ⛽️    |                                                          |
| F01.FR.16 | ⛽️    |                                                          |
| F01.FR.17 | ⛽️    |                                                          |
| F01.FR.18 | ⛽️    |                                                          |
| F01.FR.19 | ⛽️    |                                                          |
| F01.FR.20 | ✅     | Currently when no EVSE ID is given, request is rejected. |
| F01.FR.21 | ✅     |                                                          |
| F01.FR.22 | ✅     |                                                          |
| F01.FR.23 | ✅     |                                                          |
| F01.FR.24 | ✅     |                                                          |
| F01.FR.25 | ⛽️    |                                                          |
| F01.FR.26 |        |                                                          |

## RemoteControl - Remote Start Transaction - Remote Start First

| ID        | Status | Remark                                                   |
| --------- | ------ | -------------------------------------------------------- |
| F02.FR.01 | ⛽️    |                                                          |
| F02.FR.02 | ⛽️    |                                                          |
| F02.FR.03 | ⛽️    |                                                          |
| F02.FR.04 | ⛽️    |                                                          |
| F02.FR.05 | ⛽️    |                                                          |
| F02.FR.06 | ⛽️    |                                                          |
| F02.FR.07 | ⛽️    |                                                          |
| F02.FR.08 | ⛽️    |                                                          |
| F02.FR.09 | ⛽️    |                                                          |
| F02.FR.10 | ⛽️    |                                                          |
| F02.FR.11 | ⛽️❓  | Charging station or libocpp?                             |
| F02.FR.12 | ⛽️    |                                                          |
| F02.FR.13 | ⛽️    |                                                          |
| F02.FR.14 | ✅     |                                                          |
| F02.FR.15 | ✅     | Currently always rejected                                |
| F02.FR.16 |        |                                                          |
| F02.FR.17 |        |                                                          |
| F02.FR.18 |        |                                                          |
| F02.FR.19 |        |                                                          |
| F02.FR.20 |        |                                                          |
| F02.FR.21 | ⛽️    |                                                          |
| F02.FR.22 | ✅     | Currently when no EVSE ID is given, request is rejected. |
| F02.FR.23 | ✅     |                                                          |
| F02.FR.24 | ✅     |                                                          |
| F02.FR.25 | ✅     |                                                          |
| F02.FR.26 | ✅     |                                                          |
| F02.FR.27 |        |                                                          |

## RemoteControl - Remote Stop Transaction

| ID        | Status | Remark                                                        |
| --------- | ------ | ------------------------------------------------------------- |
| F03.FR.01 | ✅     |                                                               |
| F03.FR.02 | ⛽️    | The Charging Station should send a `TransactionEventRequest`. |
| F03.FR.03 | ⛽️    |                                                               |
| F03.FR.04 | ⛽️    |                                                               |
| F03.FR.05 | ⛽️    |                                                               |
| F03.FR.06 | ⛽️    |                                                               |
| F03.FR.07 | ✅     |                                                               |
| F03.FR.08 | ✅     |                                                               |
| F03.FR.09 | ⛽️    |                                                               |

## RemoteControl - Remote Stop ISO 15118 Charging from CSMS

| ID        | Status | Remark |
| --------- | ------ | ------ |
| F04.FR.01 | ❎     |        |
| F04.FR.02 | ✅     |        |
| F04.FR.03 | ✅     |        |
| F04.FR.04 | ✅     |        |
| F04.FR.05 |        |        |
| F04.FR.06 |        |        |

## RemoteControl - Remotely Unlock Connector

| ID        | Status | Remark |
| --------- | ------ | ------ |
| F05.FR.01 | ✅     |        |
| F05.FR.02 | ✅     |        |
| F05.FR.03 | ✅     |        |
| F05.FR.04 | ⛽️    |        |
| F05.FR.05 | ⛽️    |        |
| F05.FR.06 | ⛽️    |        |

## RemoteControl - Trigger Message

| ID                    | Status | Remark |
| --------------------- | ------ | ------ |
| F06.FR.01             | ❎     |        |
| F06.FR.02             | ❎     |        |
| F06.FR.03             | ✅     |        |
| F06.FR.04             | ✅     |        |
| F06.FR.05             | ✅     |        |
| F06.FR.06             | ✅     |        |
| F06.FR.07             | ✅     |        |
| F06.FR.08             | ✅     |        |
| F06.FR.09             | ✅     |        |
| F06.FR.10             | ✅     |        |
| F06.FR.11             | ✅     |        |
| F06.FR.12             | ✅     |        |
| F06.FR.13             | ✅     |        |
| F06.FR.14             | ✅     |        |
| F06.FR.15             | ✅     |        |
| F06.FR.16             | ✅     |        |
| F06.FR.17             | ✅     |        |
| F06.FR.18 <br> (2.1 ) |        |        |
| F06.FR.19 <br> (2.1 ) |        |        |

## RemoteControl - Remote start transactions with fixed cost, energy or time (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| F07.FR.01 |        |        |
| F07.FR.02 |        |        |
| F07.FR.03 |        |        |
| F07.FR.04 |        |        |

## Availability - Status Notification

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| G01.FR.01            | ✅     |        |
| G01.FR.02            | ⛽️❓  |        |
| G01.FR.03            | ✅     |        |
| G01.FR.04            | ✅     |        |
| G01.FR.05            | ✅     |        |
| G01.FR.06            |        |        |
| G01.FR.07            | ✅     |        |
| G01.FR.08            | ⛽️❓  |        |
| G01.FR.09 <br> (2.1) |        |        |

## Availability - Heartbeat

| ID        | Status | Remark                                    |
| --------- | ------ | ----------------------------------------- |
| G02.FR.01 | ✅     |                                           |
| G02.FR.02 | ✅     |                                           |
| G02.FR.03 | ❎     |                                           |
| G02.FR.04 | ❎     |                                           |
| G02.FR.05 |        | Not mandatory, so we can leave like this. |
| G02.FR.06 | ✅     |                                           |
| G02.FR.07 |        |                                           |

## Availability - Change Availability EVSE/Connector

| ID        | Status | Remark |
| --------- | ------ | ------ |
| G03.FR.01 | ✅     |        |
| G03.FR.02 | ✅     |        |
| G03.FR.03 | ✅     |        |
| G03.FR.04 | ✅     |        |
| G03.FR.05 | ✅     |        |
| G03.FR.06 | ✅     |        |
| G03.FR.07 | ✅     |        |
| G03.FR.08 | ✅     |        |

## Availability - Change Availability Charging Station

| ID        | Status | Remark |
| --------- | ------ | ------ |
| G04.FR.01 | ⛽️❓  |        |
| G04.FR.02 | ✅     |        |
| G04.FR.03 | ✅     |        |
| G04.FR.04 | ✅     |        |
| G04.FR.05 | ⛽️    |        |
| G04.FR.06 | ✅     |        |
| G04.FR.07 | ✅     |        |
| G04.FR.08 | ✅     |        |
| G04.FR.09 | ⛽️    |        |

## Availability - Lock Failure

| ID        | Status | Remark |
| --------- | ------ | ------ |
| G05.FR.01 | ⛽️❓  |        |
| G05.FR.02 | ⛽️❓  |        |
| G05.FR.03 | 🌐     |        |
| G05.FR.04 | ⛽️    |        |

## Reservation - Reservation

| ID        | Status | Remark |
| --------- | ------ | ------ |
| H01.FR.01 | ✅     |        |
| H01.FR.02 | ✅     |        |
| H01.FR.03 | ✅     |        |
| H01.FR.04 | ✅     |        |
| H01.FR.06 | ✅     |        |
| H01.FR.07 | ✅     |        |
| H01.FR.09 | ✅     |        |
| H01.FR.11 | ✅     |        |
| H01.FR.12 | ✅     |        |
| H01.FR.14 | ✅     |        |
| H01.FR.15 | ✅     |        |
| H01.FR.16 | ✅     |        |
| H01.FR.17 | ✅     |        |
| H01.FR.18 | ✅     |        |
| H01.FR.19 | ✅     |        |
| H01.FR.20 | ⛽️    |        |
| H01.FR.23 | ⛽️    |        |
| H01.FR.24 | ⛽️    |        |

## Reservation - Cancel Reservation

| ID        | Status | Remark |
| --------- | ------ | ------ |
| H02.FR.01 | ✅     |        |
| H02.FR.02 | ✅     |        |

## Reservation - Use a reserved EVSE

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| H03.FR.01            | ✅     |        |
| H03.FR.02            | ✅     |        |
| H03.FR.03            | ✅     |        |
| H03.FR.04            | ✅     |        |
| H03.FR.05            | ✅     |        |
| H03.FR.06            | ✅     |        |
| H03.FR.07            | ⛽️    |        |
| H03.FR.08            | ⛽️    |        |
| H03.FR.09            | ✅     |        |
| H03.FR.10            | ✅     |        |
| H03.FR.11 <br> (2.1) |        |        |

## Reservation - Reservation Ended, not used

| ID        | Status | Remark |
| --------- | ------ | ------ |
| H04.FR.01 | ✅     |        |
| H04.FR.02 | ✅     |        |
| H04.FR.03 | ⛽️    |        |

## TariffAndCost - Show EV Driver-specific Tariff Information

| ID        | Status | Remark |
| --------- | ------ | ------ |
| I01.FR.01 | 🌐     |        |
| I01.FR.02 | 🌐     |        |
| I01.FR.03 | ⛽️    |        |

## TariffAndCost - Show EV Driver Running Total Cost During Charging

| ID        | Status | Remark |
| --------- | ------ | ------ |
| I02.FR.01 | 🌐     |        |
| I02.FR.02 | ✅     |        |
| I02.FR.03 | ⛽️    |        |
| I02.FR.04 | ⛽️    |        |

## TariffAndCost - Show EV Driver Final Total Cost After Charging

| ID        | Status | Remark |
| --------- | ------ | ------ |
| I03.FR.01 | ✅     |        |
| I03.FR.02 | 🌐     |        |
| I03.FR.03 | ⛽️    |        |
| I03.FR.04 | 🌐     |        |
| I03.FR.05 | ⛽️    |        |

## TariffAndCost - Show Fallback Tariff Information

| ID        | Status | Remark |
| --------- | ------ | ------ |
| I04.FR.01 | ⛽️    |        |
| I04.FR.02 | 🌐     |        |

## TariffAndCost - Show Fallback Total Cost Message

| ID        | Status | Remark |
| --------- | ------ | ------ |
| I05.FR.01 | 🌐     |        |
| I05.FR.02 | ⛽️    |        |

## TariffAndCost - Update Tariff Information During Transaction

| ID        | Status | Remark |
| --------- | ------ | ------ |
| I06.FR.01 | 🌐     |        |
| I06.FR.02 | 🌐     |        |
| I06.FR.03 | ⛽️    |        |

## TariffAndCost - Set Default Tariff (New in OCPP 2.1)

| ID        | Status            | Remark |
| --------- | ----------------- | ------ |
| I07.FR.01 |                   |        |
| I07.FR.02 |                   |        |
| I07.FR.03 |                   |        |
| I07.FR.04 |                   |        |
| I07.FR.05 |                   |        |
| I07.FR.06 |                   |        |
| I07.FR.10 |                   |        |
| I07.FR.11 |                   |        |
| I07.FR.12 |                   |        |
| I07.FR.13 |                   |        |
| I07.FR.14 |                   |        |
| I07.FR.17 |                   |        |
| I07.FR.18 |                   |        |
| I07.FR.19 |                   |        |
| I07.FR.20 |                   |        |
| I07.FR.21 |                   |        |
| I07.FR.22 |                   |        |
| I07.FR.23 |                   |        |
| I07.FR.24 |                   |        |
|           | Tarrif acceptance |        |
| I07.FR.30 |                   |        |
| I07.FR.31 |                   |        |
| I07.FR.32 |                   |        |

## TariffAndCost - Receive Driver Tariff (New in OCPP 2.1)

| ID        | Status                                    | Remark |
| --------- | ----------------------------------------- | ------ |
| I08.FR.01 |                                           |        |
| I08.FR.04 |                                           |        |
| I08.FR.05 |                                           |        |
| I08.FR.06 |                                           |        |
| I08.FR.07 |                                           |        |
| I08.FR.08 |                                           |        |
| I08.FR.09 |                                           |        |
| I08.FR.10 |                                           |        |
|           | Tarrif acceptance                         |        |
| I08.FR.20 |                                           |        |
| I08.FR.21 |                                           |        |
| I08.FR.22 |                                           |        |
|           | Failure to process driver-specific tarrif |        |
| I08.FR.30 |                                           |        |
| I08.FR.31 |                                           |        |
| I08.FR.31 |                                           |        |
| I08.FR.32 |                                           |        |
| I08.FR.33 |                                           |        |
| I08.FR.34 |                                           |        |
| I08.FR.35 |                                           |        |
| I08.FR.36 |                                           |        |

## TariffAndCost - Get Tariffs (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| I09.FR.01 |        |        |
| I09.FR.02 |        |        |
| I09.FR.03 |        |        |
| I09.FR.04 |        |        |
| I09.FR.05 |        |        |
| I09.FR.06 |        |        |
| I09.FR.07 |        |        |

## TariffAndCost - Clear Tariffs (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| I10.FR.01 |        |        |
| I10.FR.02 |        |        |
| I10.FR.03 |        |        |
| I10.FR.05 |        |        |
| I10.FR.06 |        |        |
| I10.FR.07 |        |        |

## TariffAndCost - Change Transaction Tarrif (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| I11.FR.01 |        |        |
| I11.FR.02 |        |        |
| I11.FR.03 |        |        |
| I11.FR.04 |        |        |
| I11.FR.05 |        |        |
| I11.FR.06 |        |        |
| I11.FR.07 |        |        |
| I11.FR.08 |        |        |

## TariffAndCost - Cost Details of Transaction (New in OCPP 2.1)

| ID        | Status            | Remark |
| --------- | ----------------- | ------ |
| I12.FR.01 |                   |        |
| I12.FR.02 |                   |        |
| I12.FR.03 |                   |        |
| I12.FR.04 |                   |        |
| I12.FR.05 |                   |        |
| I12.FR.06 |                   |        |
| I12.FR.07 |                   |        |
| I12.FR.08 |                   |        |
| I12.FR.09 |                   |        |
| I12.FR.10 |                   |        |
| I12.FR.11 |                   |        |
| I12.FR.12 |                   |        |
| I12.FR.13 |                   |        |
| I12.FR.14 |                   |        |
| I12.FR.15 |                   |        |
| I12.FR.16 |                   |        |
| I12.FR.17 |                   |        |
| I12.FR.18 |                   |        |
| I12.FR.19 |                   |        |
|           | Tarrif evaluation |        |
| I12.FR.30 |                   |        |
| I12.FR.31 |                   |        |
| I12.FR.32 |                   |        |
| I12.FR.33 |                   |        |
| I12.FR.34 |                   |        |
| I12.FR.35 |                   |        |
| I12.FR.36 |                   |        |
| I12.FR.37 |                   |        |
| I12.FR.38 |                   |        |
| I12.FR.39 |                   |        |
| I12.FR.40 |                   |        |
| I12.FR.41 |                   |        |
| I12.FR.42 |                   |        |
| I12.FR.43 |                   |        |
| I12.FR.44 |                   |        |

## MeterValues - Sending Meter Values not related to a transaction

| ID                   | Status | Remark                                       |
| -------------------- | ------ | -------------------------------------------- |
| J01.FR.01            | ✅     |                                              |
| J01.FR.02            | ✅     |                                              |
| J01.FR.03            | ✅     |                                              |
| J01.FR.04            | ✅     |                                              |
| J01.FR.05            | ✅     |                                              |
| J01.FR.06            | ✅     |                                              |
| J01.FR.07            | ✅     |                                              |
| J01.FR.08            | ✅     |                                              |
| J01.FR.09            | ❎     | Location is provided by `libocpp` user.      |
| J01.FR.10            | ✅     |                                              |
| J01.FR.11            | ✅     |                                              |
| J01.FR.13            |        | Added phase rotation configuration variable. |
| J01.FR.14            | ✅     |                                              |
| J01.FR.15            | ❎     | tbd                                          |
| J01.FR.17            | ✅     |                                              |
| J01.FR.18            | ✅     |                                              |
| J01.FR.19            | ✅     |                                              |
| J01.FR.20            | ✅     |                                              |
| J01.FR.21            | ❎     | not valid                                    |
| J01.FR.22 <br> (2.1) |        |                                              |

## MeterValues - Sending transaction related Meter Values

| ID                   | Status | Remark                                       |
| -------------------- | ------ | -------------------------------------------- |
| J02.FR.01            | ✅     |                                              |
| J02.FR.02            | ✅     |                                              |
| J02.FR.03            | ✅     |                                              |
| J02.FR.04            | ✅     |                                              |
| J02.FR.05            | ✅     |                                              |
| J02.FR.06            | ✅     |                                              |
| J02.FR.07            | ✅     |                                              |
| J02.FR.09            |        | Added phase rotation configuration variable. |
| J02.FR.10            | ✅     |                                              |
| J02.FR.11            | ✅     |                                              |
| J02.FR.12 <br> (2.1) | ❎     | tbd                                          |
| J02.FR.13            | ❎     | tbd                                          |
| J02.FR.14            | ❎     | tbd                                          |
| J02.FR.16            | ❎     |                                              |
| J02.FR.17            | ❎     | tbd                                          |
| J02.FR.18            | ✅     |                                              |
| J02.FR.19            | ✅     |                                              |
| J02.FR.20            | ✅     |                                              |
| J02.FR.21            | ❎     | Signed meter values are not yet applicable.  |
| J02.FR.22 <br> (2.1) |        |                                              |
| J02.FR.23 <br> (2.1) |        |                                              |
| J02.FR.24 <br> (2.1) |        |                                              |

## MeterValues - Charging Loop with metering information exchange

| ID        | Status | Remark |
| --------- | ------ | ------ |
| J03.FR.04 |        |        |

## SmartCharging - SetChargingProfile

| ID                    | Status | Remark                                                                                                                 |
| --------------------- | ------ | ---------------------------------------------------------------------------------------------------------------------- |
| K01.FR.01             | 🌐     | `TxProfile`s are supported.                                                                                            |
| K01.FR.02             | 🌐     |                                                                                                                        |
| K01.FR.03             | 🌐 💂  | `TxProfile`s without `transactionId`s are rejected.                                                                    |
| K01.FR.04             | ✅     |                                                                                                                        |
| K01.FR.05             | ✅     |                                                                                                                        |
| K01.FR.06             | 🌐 💂  | As part of validation any `ChargingProile` with a stackLevel - chargingProfilePurpose - evseId combination is rejected |
| K01.FR.07             | ⛽️    | K08 - Notified through the `signal_set_charging_profiles` callback.                                                    |
| K01.FR.08             | 🌐     | `TxDefaultProfile`s are supported.                                                                                     |
| K01.FR.09             | ✅     |                                                                                                                        |
| K01.FR.10             | ⛽️    | K08 - During validation `validFrom` and `validTo` are set if they are blank to support this                            |
| K01.FR.11             | ❎     | K08 - The application of `ChargingProfileSchedules` are done via the `CompositeSchedule` from `GetCompositeSchedule`   |
| K01.FR.12             | ❎     | K08 - The application of `ChargingProfileSchedules` are done via the `CompositeSchedule` from `GetCompositeSchedule`   |
| K01.FR.13             | ❎     | K08 - The application of `ChargingProfileSchedules` are done via the `CompositeSchedule` from `GetCompositeSchedule`   |
| K01.FR.14             | ✅     |                                                                                                                        |
| K01.FR.15             | ✅     |                                                                                                                        |
| K01.FR.16             | ✅     |                                                                                                                        |
| K01.FR.17             | ⛽️    | K08 - The application of `ChargingProfileSchedules` are done via the `CompositeSchedule` from `GetCompositeSchedule`   |
| K01.FR.19             | ✅     |                                                                                                                        |
| K01.FR.20             | ✅     | Suggests `ACPhaseSwitchingSupported` should be per EVSE, conflicting with the rest of the spec.                        |
| K01.FR.21             |        | There is an active community discussion on this topic.                                                                 |
| K01.FR.22             |        |                                                                                                                        |
| K01.FR.26             | ✅     |                                                                                                                        |
| K01.FR.27 <br> (2.1)  | ✅     |                                                                                                                        |
| K01.FR.28             | ✅     |                                                                                                                        |
| K01.FR.29             | ✅     |                                                                                                                        |
| K01.FR.30             | ⛽️    | K08 - The application of `ChargingProfileSchedules` are done via the `CompositeSchedule` from `GetCompositeSchedule`   |
| K01.FR.31             | ✅     |                                                                                                                        |
| K01.FR.32             | ⛽️    | K08 - The application of `ChargingProfileSchedules` are done via the `CompositeSchedule` from `GetCompositeSchedule`   |
| K01.FR.33             | ✅     |                                                                                                                        |
| K01.FR.34             |        | Defer to K15 - K17 work                                                                                                |
| K01.FR.35             | ✅     |                                                                                                                        |
| K01.FR.36             | ⛽️    | K08                                                                                                                    |
| K01.FR.37             | ⛽️    | K08                                                                                                                    |
| K01.FR.38             | ✅     | `ChargingStationMaxProfile`s with `Relative` for `chargingProfileKind` are rejected.                                   |
| K01.FR.39             | ✅     | New `TxProfile`s matching existing `(stackLevel, transactionId)` are rejected.                                         |
| K01.FR.40             | ✅     | `Absolute`/`Recurring` profiles without `startSchedule` fields are rejected.                                           |
| K01.FR.41             | ✅     | `Relative` profiles with `startSchedule` fields are rejected.                                                          |
| K01.FR.42 <br> (2.1)  | ⛽️    |                                                                                                                        |
| K01.FR.43             |        | Open question to OCA - https://oca.causewaynow.com/wg/OCA-TWG/mail/thread/4254                                         |
| K01.FR.44 <br> (2.1)  | ✅     | We reject invalid profiles instead of modifying and accepting them.                                                    |
| K01.FR.45             | ✅     | We reject invalid profiles instead of modifying and accepting them.                                                    |
| K01.FR.46             | ⛽️    | K08                                                                                                                    |
| K01.FR.47             | ⛽️    | K08                                                                                                                    |
| K01.FR.48             | ✅     |                                                                                                                        |
| K01.FR.49             | ✅     |                                                                                                                        |
| K01.FR.50             | ⛽️    | K08                                                                                                                    |
| K01.FR.51             | ⛽️    | K08                                                                                                                    |
| K01.FR.52             | ✅     |                                                                                                                        |
| K01.FR.53             | ✅     |                                                                                                                        |
| K01.FR.54 <br> (2.1)  | ✅     |                                                                                                                        |
| K01.FR.55 <br> (2.1)  | ✅     |                                                                                                                        |
| K01.FR.56 <br> (2.1)  | ✅      |                                                                                                                        |
|                       |        | PriorityCharging                                                                                                       |
| K01.FR.70 <br> (2.1)  | ✅      |                                                                                                                        |
| K01.FR.71 <br> (2.1)  | ✅      |                                                                                                                        |
|                       |        | Max External Constraints Id                                                                                            |
| K01.FR.80 <br> (2.1)  | 🌐      |                                                                                                                        |
| K01.FR.81 <br> (2.1)  | ✅      |                                                                                                                        |
| K01.FR.82 <br> (2.1)  |        |                                                                                                                        |
|                       |        | Use Local Time / Randomized Delay                                                                                      |
| K01.FR.90 <br> (2.1)  |        |                                                                                                                        |
| K01.FR.91 <br> (2.1)  |        |                                                                                                                        |
| K01.FR.92 <br> (2.1)  |        |                                                                                                                        |
| K01.FR.93 <br> (2.1)  |        |                                                                                                                        |
| K01.FR.94 <br> (2.1)  |        |                                                                                                                        |
| K01.FR.95 <br> (2.1)  | ✅      |                                                                                                                        |
|                       |        | Limit Beyond SoC / Offline validity                                                                                    |
| K01.FR.100 <br> (2.1) |        |                                                                                                                        |
| K01.FR.101 <br> (2.1) |        |                                                                                                                        |
| K01.FR.102 <br> (2.1) |        |                                                                                                                        |
| K01.FR.103 <br> (2.1) |        |                                                                                                                        |
|                       |        | OperationMode                                                                                                          |
| K01.FR.110 <br> (2.1) |        |                                                                                                                        |
|                       |        | Checking optional support                                                                                              |
| K01.FR.120 <br> (2.1) | ✅      |                                                                                                                        |
| K01.FR.121 <br> (2.1) | ✅      |                                                                                                                        |
| K01.FR.122 <br> (2.1) | ✅      |                                                                                                                        |
| K01.FR.123 <br> (2.1) | ✅      |                                                                                                                        |
| K01.FR.124 <br> (2.1) | ✅      |                                                                                                                        |
| K01.FR.125 <br> (2.1) | ✅      |                                                                                                                        |
| K01.FR.126 <br> (2.1) | ✅      |                                                                                                                        |

## SmartCharging - Central Smart Charging

| ID        | Status | Remark                                           |
| --------- | ------ | ------------------------------------------------ |
| K02.FR.01 | ❎     |                                                  |
| K02.FR.02 | ❎     | This should be handled by the user of `libocpp`. |
| K02.FR.03 | ❎     |                                                  |
| K02.FR.04 | ✅     |                                                  |
| K02.FR.05 | ✅     |                                                  |
| K02.FR.06 |        | The same as K01.FR.21                            |
| K02.FR.07 |        | The same as K01.FR.22                            |
| K02.FR.08 |        |                                                  |

## SmartCharging - Local Smart Charging

| ID        | Status | Remark                |
| --------- | ------ | --------------------- |
| K03.FR.01 | ❎     |                       |
| K03.FR.02 |        |                       |
| K03.FR.03 |        |                       |
| K03.FR.04 |        |                       |
| K03.FR.05 |        |                       |
| K03.FR.06 |        |                       |
| K03.FR.07 |        | The same as K01.FR.21 |
| K03.FR.08 |        | The same as K01.FR.22 |

## SmartCharging - Internal Load Balancing

| ID                   | Status | Remark                                           |
| -------------------- | ------ | ------------------------------------------------ |
| K04.FR.01            | ✅     |                                                  |
| K04.FR.02            |        |                                                  |
| K04.FR.03            | ✅     |                                                  |
| K04.FR.04            |        | The same as K01.FR.21                            |
| K04.FR.05 <br> (2.1) |        | This should be handled by the user of `libocpp`. |

## SmartCharging - Remote Start Transaction with Charging Profile

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K05.FR.01 | ❎     |        |
| K05.FR.02 | ✅     |        |
| K05.FR.03 | ✅     |        |
| K05.FR.04 | ✅     |        |
| K05.FR.05 | ✅     |        |

## SmartCharging - Offline Behavior Smart Charging During Transaction

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K06.FR.01 |        |        |
| K06.FR.02 |        |        |

## SmartCharging - Offline Behavior Smart Charging at Start of Transaction

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K07.FR.01 |        |        |

## SmartCharging - Get Composite Schedule

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| K08.FR.01            | ✅     |        |
| K08.FR.02            | ✅     |        |
| K08.FR.03            | ✅     |        |
| K08.FR.04 <br> (2.1) | ✅     |        |
| K08.FR.05            | ✅     |        |
| K08.FR.06 <br> (2.1) | ✅     |        |
| K08.FR.07            | ✅     |        |
| K08.FR.08 <br> (2.1) | ✅     |        |

## SmartCharging - Get Charging Profiles

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K09.FR.01 | ✅     |        |
| K09.FR.02 | ✅     |        |
| K09.FR.03 | 🌐     |        |
| K09.FR.04 | ✅     |        |
| K09.FR.05 | ✅     |        |
| K09.FR.06 | ✅     |        |

## SmartCharging - Clear Charging Profile

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| K10.FR.01            | ✅     |        |
| K10.FR.02            | 🌐     |        |
| K10.FR.03            | ✅     |        |
| K10.FR.04 <br> (2.1) |        |        |
| K10.FR.05            | ⛽️    |        |
| K10.FR.06 <br> (2.1) | 🌐     |        |
| K10.FR.07            | ⛽️    |        |
| K10.FR.08 <br> (2.1) |        |        |
| K10.FR.09 <br> (2.1) |        |        |

## SmartCharging - Set / Update External Charging Limit With Ongoing Transaction

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| K11.FR.01 <br> (2.1) |        |        |
| K11.FR.02            |        |        |
| K11.FR.03            |        |        |
| K11.FR.04            |        |        |
| K11.FR.05            |        |        |
| K11.FR.06 <br> (2.1) |        |        |
| K11.FR.07 <br> (2.1) |        |        |

## SmartCharging - Set / Update External Charging Limit Without Ongoing Transaction

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K12.FR.01 |        |        |
| K12.FR.02 |        |        |
| K12.FR.03 |        |        |
| K12.FR.04 |        |        |
| K12.FR.05 |        |        |

## SmartCharging - Reset / Release External Charging Limit

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K13.FR.01 |        |        |
| K13.FR.02 |        |        |
| K13.FR.03 |        |        |

## SmartCharging - External Charging Limit with Local Controller

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K14.FR.01 |        |        |
| K14.FR.02 |        |        |
| K14.FR.03 |        |        |
| K14.FR.04 |        |        |
| K14.FR.05 |        |        |
| K14.FR.06 |        |        |

## SmartCharging - Charging with load leveling based on High Level Communication

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| K15.FR.01            | ✅     |        |
| K15.FR.02            | 🌐     |        |
| K15.FR.03            | 🌐     |        |
| K15.FR.04            | 🌐     |        |
| K15.FR.05            | 🌐     |        |
| K15.FR.06            | ⛽️    |        |
| K15.FR.07            | 🌐     |        |
| K15.FR.08            | 🌐     |        |
| K15.FR.09            | ⛽️    |        |
| K15.FR.10            |        |        |
| K15.FR.11            |        |        |
| K15.FR.12            |        |        |
| K15.FR.13            | 🌐     |        |
| K15.FR.14            |        |        |
| K15.FR.15            | ✅     |        |
| K15.FR.16            |        |        |
| K15.FR.17            |        |        |
| K15.FR.18            | 🌐     |        |
| K15.FR.19            |        |        |
| K15.FR.20 <br> (2.1) |        |        |
| K15.FR.21 <br> (2.1) |        |        |


## SmartCharging - Renegotiation initiated by CSMS

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| K16.FR.01            |        |        |
| K16.FR.02 <br> (2.1) |        |        |
| K16.FR.03            |        |        |
| K16.FR.04 <br> (2.1) |        |        |
| K16.FR.05 <br> (2.1) |        |        |
| K16.FR.06            |        |        |
| K16.FR.07            |        |        |
| K16.FR.08            |        |        |
| K16.FR.09            |        |        |
| K16.FR.10            |        |        |
| K16.FR.11 <br> (2.1) |        |        |
| K16.FR.12            |        |        |
| K16.FR.13 <br> (2.1) |        |        |
| K16.FR.14 <br> (2.1) |        |        |

## SmartCharging - Renegotiation initiated by EV

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| K17.FR.01            |        |        |
| K17.FR.02            |        |        |
| K17.FR.03            |        |        |
| K17.FR.04            |        |        |
| K17.FR.05            |        |        |
| K17.FR.06 <br> (2.1) |        |        |
| K17.FR.07 <br> (2.1) |        |        |
| K17.FR.08            |        |        |
| K17.FR.09            |        |        |
| K17.FR.10            |        |        |
| K17.FR.11 <br> (2.1) |        |        |
| K17.FR.12 <br> (2.1) |        |        |
| K17.FR.13            |        |        |
| K17.FR.14            |        |        |
| K17.FR.15            |        |        |
| K17.FR.16 <br> (2.1) |        |        |
| K17.FR.17            |        |        |
| K17.FR.18 <br> (2.1) |        |        |
| K17.FR.19 <br> (2.1) |        |        |

## SmartCharging - ISO 15118-20 Scheduled Control Mode (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K18.FR.01 |        |        |
| K18.FR.02 |        |        |
| K18.FR.03 |        |        |
| K18.FR.04 |        |        |
| K18.FR.05 |        |        |
| K18.FR.06 |        |        |
| K18.FR.07 |        |        |
| K18.FR.08 |        |        |
| K18.FR.09 |        |        |
| K18.FR.10 |        |        |
| K18.FR.11 |        |        |
| K18.FR.12 |        |        |
| K18.FR.13 |        |        |
| K18.FR.14 |        |        |
| K18.FR.15 |        |        |
| K18.FR.16 |        |        |
| K18.FR.17 |        |        |
| K18.FR.18 |        |        |
| K18.FR.19 |        |        |
| K18.FR.20 |        |        |
| K18.FR.21 |        |        |
| K18.FR.22 |        |        |
| K18.FR.23 |        |        |

## SmartCharging - ISO 15118-20 Dynamic Control Mode (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K19.FR.01 |        |        |
| K19.FR.02 |        |        |
| K19.FR.03 |        |        |
| K19.FR.04 |        |        |
| K19.FR.05 |        |        |
| K19.FR.06 |        |        |
| K19.FR.07 |        |        |
| K19.FR.08 |        |        |
| K19.FR.09 |        |        |
| K19.FR.10 |        |        |
| K19.FR.11 |        |        |
| K19.FR.12 |        |        |
| K19.FR.13 |        |        |
| K19.FR.14 |        |        |
| K19.FR.15 |        |        |
| K19.FR.16 |        |        |

## SmartCharging - ISO 15118-20 Adjusting charging schedule when energy needs change (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K20.FR.01 |        |        |
| K20.FR.02 |        |        |
| K20.FR.03 |        |        |
| K20.FR.04 |        |        |
| K20.FR.05 |        |        |

## SmartCharging - Requesting priority charging remotely (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K21.FR.01 |        |        |
| K21.FR.02 |        |        |
| K21.FR.03 |        |        |
| K21.FR.04 |        |        |
| K21.FR.05 |        |        |
| K21.FR.06 |        |        |
| K21.FR.07 |        |        |
| K21.FR.08 |        |        |
| K21.FR.09 |        |        |

## SmartCharging - Requesting priority charging locally (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K22.FR.01 |        |        |
| K22.FR.02 |        |        |
| K22.FR.03 |        |        |
| K22.FR.04 |        |        |
| K22.FR.05 |        |        |
| K22.FR.06 |        |        |

## SmartCharging - Smart Charging with EMS and LocalGeneration (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K27.FR.01 |        |        |
| K27.FR.02 |        |        |
| K27.FR.03 |        |        |
| K27.FR.04 |        |        |
| K27.FR.05 |        |        |

## SmartCharging - Dynamic charging profiles from CSMS (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K28.FR.01 |        |        |
| K28.FR.02 |        |        |
| K28.FR.03 |        |        |
| K28.FR.04 |        |        |
| K28.FR.05 |        |        |
| K28.FR.06 |        |        |
| K28.FR.07 |        |        |
| K28.FR.08 |        |        |
| K28.FR.09 |        |        |
| K28.FR.10 |        |        |
| K28.FR.11 |        |        |
| K28.FR.12 |        |        |

## SmartCharging - Dynamic charging profiles by external system (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| K29.FR.01 |        |        |
| K29.FR.02 |        |        |
| K29.FR.03 |        |        |
| K29.FR.04 |        |        |
| K29.FR.05 |        |        |
| K29.FR.06 |        |        |

## FirmwareManagement - Secure Firmware Update

| ID                   | Status | Remark                                      |
| -------------------- | ------ | ------------------------------------------- |
| L01.FR.01            | ⛽️    |                                             |
| L01.FR.02            | ✅     | Security Notification is sent by `libocpp`. |
| L01.FR.03            | ✅     | Security Notification is sent by `libocpp`. |
| L01.FR.04            | ⛽️    |                                             |
| L01.FR.05            | ⛽️    |                                             |
| L01.FR.06            | ⛽️    |                                             |
| L01.FR.07            | ⛽️    |                                             |
| L01.FR.08            | ❎     | Recommendation, not a requirement           |
| L01.FR.09            | 🤓     | Requirement on the firmware file itself.    |
| L01.FR.10            | ⛽️    |                                             |
| L01.FR.11            | 🌐     |                                             |
| L01.FR.12            | ⛽️    |                                             |
| L01.FR.13            | ⛽️    |                                             |
| L01.FR.14            | ⛽️    |                                             |
| L01.FR.15            | ⛽️    |                                             |
| L01.FR.16            | ⛽️    |                                             |
| L01.FR.20            | ✅     |                                             |
| L01.FR.21            | ⛽️    |                                             |
| L01.FR.22            | ⛽️    |                                             |
| L01.FR.23            | ⛽️    |                                             |
| L01.FR.24            | ⛽️    |                                             |
| L01.FR.25            | ✅     |                                             |
| L01.FR.26            | ✅     |                                             |
| L01.FR.27            |        | Optional requirement                        |
| L01.FR.28            | ⛽️    |                                             |
| L01.FR.29            | ⛽️    |                                             |
| L01.FR.30            | ⛽️    |                                             |
| L01.FR.31            | ✅     |                                             |
| L01.FR.32            | ❎     | Optional requirement                        |
| L01.FR.33 <br> (2.1) |        |                                             |
| L01.FR.34 <br> (2.1) |        |                                             |

## FirmwareManagement - Non-Secure Firmware Update

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| L02.FR.01            | ⛽️    |        |
| L02.FR.02            | ⛽️    |        |
| L02.FR.03            | ⛽️    |        |
| L02.FR.04            | ⛽️    |        |
| L02.FR.05            | ⛽️    |        |
| L02.FR.06            | ⛽️    |        |
| L02.FR.07            | ⛽️    |        |
| L02.FR.08            | ⛽️    |        |
| L02.FR.09            | ⛽️    |        |
| L02.FR.10            | ⛽️    |        |
| L02.FR.14            | ⛽️    |        |
| L02.FR.15            | ⛽️    |        |
| L02.FR.16            | ✅     |        |
| L02.FR.17            | ✅     |        |
| L02.FR.18            | ⛽️    |        |
| L02.FR.19            | ⛽️    |        |
| L02.FR.20            | ⛽️    |        |
| L02.FR.21            | ⛽️    |        |
| L02.FR.22 <br> (2.1) |        |        |
| L02.FR.23 <br> (2.1) |        |        |

## FirmwareManagement - Publish Firmware file on Local Controller

| ID        | Status | Remark |
| --------- | ------ | ------ |
| L03.FR.01 |        |        |
| L03.FR.02 |        |        |
| L03.FR.03 |        |        |
| L03.FR.04 |        |        |
| L03.FR.05 |        |        |
| L03.FR.06 |        |        |
| L03.FR.07 |        |        |
| L03.FR.08 |        |        |
| L03.FR.09 |        |        |
| L03.FR.10 |        |        |
| L03.FR.11 |        |        |

## FirmwareManagement - Unpublish Firmware file on Local Controller

| ID        | Status | Remark |
| --------- | ------ | ------ |
| L04.FR.01 |        |        |
| L04.FR.02 |        |        |
| L04.FR.03 |        |        |
| L04.FR.04 |        |        |

## ISO 15118 CertificateManagement - Certificate installation EV

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| M01.FR.01            | ✅     |        |
| M01.FR.02 <br> (2.1) |        |        |
| M01.FR.03 <br> (2.1) |        |        |
| M01.FR.04 <br> (2.1) |        |        |
| M01.FR.05 <br> (2.1) |        |        |
| M01.FR.06 <br> (2.1) |        |        |
| M01.FR.07 <br> (2.1) |        |        |

## ISO 15118 CertificateManagement - Certificate Update EV

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| M02.FR.01 <br> (2.1) |        |        |
| M02.FR.02 <br> (2.1) |        |        |

## ISO 15118 CertificateManagement - Retrieve list of available certificates from a Charging Station

| ID        | Status | Remark |
| --------- | ------ | ------ |
| M03.FR.01 | ✅     |        |
| M03.FR.02 | ✅     |        |
| M03.FR.03 | ✅     |        |
| M03.FR.04 | ✅     |        |
| M03.FR.05 | ✅     |        |

## ISO 15118 CertificateManagement - Delete a specific certificate from a Charging Station

| ID        | Status | Remark                               |
| --------- | ------ | ------------------------------------ |
| M04.FR.01 | ✅     |                                      |
| M04.FR.02 | ✅     | `libevse-security` handles response. |
| M04.FR.03 | ✅     | `libevse-security` handles response. |
| M04.FR.04 | ✅     | `libevse-security` handles response. |
| M04.FR.05 | ✅     | `libevse-security` handles response. |
| M04.FR.06 | ✅     | `libevse-security` handles response. |
| M04.FR.07 | ✅     | `libevse-security` handles response. |
| M04.FR.08 | ✅     | `libevse-security` handles response. |

## ISO 15118 CertificateManagement - Install CA certificate in a Charging Station

| ID        | Status | Remark |
| --------- | ------ | ------ |
| M05.FR.01 | ✅     |        |
| M05.FR.02 | ✅     |        |
| M05.FR.03 | ✅     |        |
| M05.FR.06 |        |        |
| M05.FR.07 | ✅     |        |
| M05.FR.09 |        |        |
| M05.FR.10 |        |        |
| M05.FR.11 |        |        |
| M05.FR.12 |        |        |
| M05.FR.13 |        |        |
| M05.FR.14 |        |        |
| M05.FR.15 |        |        |
| M05.FR.16 |        |        |
| M05.FR.17 |        |        |

## ISO 15118 CertificateManagement - Get V2G Charging Station Certificate status

| ID        | Status | Remark |
| --------- | ------ | ------ |
| M06.FR.01 | ❎     |        |
| M06.FR.02 | ❎     |        |
| M06.FR.03 | ❎     |        |
| M06.FR.04 | ❎     |        |
| M06.FR.06 | ✅     |        |
| M06.FR.07 |        |        |
| M06.FR.08 | ❎     |        |
| M06.FR.09 | ❎     |        |
| M06.FR.10 | ✅     |        |

## ISO 15118 CertificateManagement - Get Vehicle Certificate Chain Revocation Status (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| M07.FR.01 |        |        |
| M07.FR.02 |        |        |
| M07.FR.03 |        |        |
| M07.FR.04 |        |        |
| M07.FR.06 |        |        |
| M07.FR.07 |        |        |
| M07.FR.08 |        |        |
| M07.FR.09 |        |        |
| M07.FR.10 |        |        |
| M07.FR.11 |        |        |

## Diagnostics - Retrieve Log Information

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| N01.FR.01            | ✅     |        |
| N01.FR.02            | ⛽️    |        |
| N01.FR.03            | ⛽️    |        |
| N01.FR.04            | ⛽️    |        |
| N01.FR.05            | ⛽️    |        |
| N01.FR.06            | ⛽️    |        |
| N01.FR.07            | ⛽️    |        |
| N01.FR.08            | ⛽️    |        |
| N01.FR.09            | ⛽️    |        |
| N01.FR.10            | ⛽️    |        |
| N01.FR.11            | ⛽️    |        |
| N01.FR.12            | ⛽️    |        |
| N01.FR.13            | ⛽️    |        |
| N01.FR.14            | ⛽️    |        |
| N01.FR.15            | ⛽️    |        |
| N01.FR.16            | ⛽️    |        |
| N01.FR.17            | ⛽️    |        |
| N01.FR.18            | ⛽️    |        |
| N01.FR.19            | ⛽️    |        |
| N01.FR.20            | ⛽️    |        |
| N01.FR.21 <br> (2.1) | ⛽️    |        |
| N01.FR.22 <br> (2.1) | ⛽️    |        |
| N01.FR.23 <br> (2.1) | ⛽️    |        |
| N01.FR.24 <br> (2.1) | ⛽️    |        |
| N01.FR.25 <br> (2.1) | ⛽️    |        |
| N01.FR.26 <br> (2.1) | 🌐     |        |
| N01.FR.27 <br> (2.1) | 🌐     |        |
| N01.FR.28 <br> (2.1) | 🌐     |        |
| N01.FR.29 <br> (2.1) | ⛽️    |        |
| N01.FR.30 <br> (2.1) | ⛽️    |        |

## Diagnostics - Get Monitoring report

| ID                   | Status | Remark                                 |
| -------------------- | ------ | -------------------------------------- |
| N02.FR.01            | ✅     |                                        |
| N02.FR.02            | ❎     | Libocpp supports all MonitoringType(s) |
| N02.FR.03            | ✅     |                                        |
| N02.FR.04            | ✅     |                                        |
| N02.FR.05            | ✅     |                                        |
| N02.FR.06            | ✅     |                                        |
| N02.FR.07            | ✅     |                                        |
| N02.FR.08            | ✅     |                                        |
| N02.FR.09            | ✅     |                                        |
| N02.FR.10            | ✅     |                                        |
| N02.FR.11            | ✅     |                                        |
| N02.FR.12            | ✅     |                                        |
| N02.FR.13            | ✅     |                                        |
| N02.FR.14            | ✅     |                                        |
| N02.FR.15            | ✅     |                                        |
| N02.FR.16            | ✅     |                                        |
| N02.FR.17            | ✅     |                                        |
| N02.FR.18            | ✅     |                                        |
| N02.FR.19            | ✅     |                                        |
| N02.FR.20            | ✅     |                                        |
| N02.FR.21            | ✅     |                                        |
| N02.FR.22 <br> (2.1) |        |                                        |
| N02.FR.23 <br> (2.1) |        |                                        |

## Diagnostics - Set Monitoring Base

| ID        | Status | Remark                                 |
| --------- | ------ | -------------------------------------- |
| N03.FR.01 | ✅     |                                        |
| N03.FR.02 | ❎     | Libocpp supports all MonitoringType(s) |
| N03.FR.03 | ✅     |                                        |
| N03.FR.04 | ✅     |                                        |
| N03.FR.05 | ✅     |                                        |

## Diagnostics - Set Variable Monitoring

| ID        | Status | Remark                                 |
| --------- | ------ | -------------------------------------- |
| N04.FR.01 | ✅     |                                        |
| N04.FR.02 | ✅     |                                        |
| N04.FR.03 | ✅     |                                        |
| N04.FR.04 | ✅     |                                        |
| N04.FR.05 | ❎     | Everything is supported on our charger |
| N04.FR.06 | ✅     |                                        |
| N04.FR.07 | ✅     |                                        |
| N04.FR.08 | ✅     |                                        |
| N04.FR.09 | ✅     |                                        |
| N04.FR.10 | ✅     |                                        |
| N04.FR.11 | ✅     |                                        |
| N04.FR.12 | ✅     |                                        |
| N04.FR.13 | ✅     |                                        |
| N04.FR.14 | ✅     |                                        |
| N04.FR.15 |        |                                        |
| N04.FR.16 | ✅     |                                        |
| N04.FR.17 | ❎     | Recommendation only                    |
| N04.FR.18 | ✅     |                                        |
| N04.FR.19 | ✅     |                                        |

## Diagnostics - Set Monitoring Level

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N05.FR.01 | ✅     |        |
| N05.FR.02 | ✅     |        |
| N05.FR.03 | ✅     |        |

## Diagnostics - Clear / Remove Monitoring

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N06.FR.01 | ✅     |        |
| N06.FR.02 | ✅     |        |
| N06.FR.03 | ✅     |        |
| N06.FR.04 | ✅     |        |
| N06.FR.05 | ✅     |        |
| N06.FR.06 | ✅     |        |
| N06.FR.07 | ✅     |        |

## Diagnostics - Alert Event

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| N07.FR.02 <br> (2.1) | ✅     |        |
| N07.FR.03            | ✅     |        |
| N07.FR.04            | ✅     |        |
| N07.FR.05            |        |        |
| N07.FR.06            | ✅     |        |
| N07.FR.07            | ✅     |        |
| N07.FR.10            | ✅     |        |
| N07.FR.11 <br> (2.1) | ✅     |        |
| N07.FR.12 <br> (2.1) | ✅     |        |
| N07.FR.13            | ✅     |        |
| N07.FR.14 <br> (2.1) |        |        |
| N07.FR.15            | ✅     |        |
| N07.FR.16            | ✅     |        |
| N07.FR.17            | ✅     |        |
| N07.FR.18            | ✅     |        |
| N07.FR.19            | ✅     |        |
| N07.FR.20 <br> (2.1) |        |        |
| N07.FR.21 <br> (2.1) |        |        |
| N07.FR.22 <br> (2.1) |        |        |
| N07.FR.23 <br> (2.1) |        |        |

## Diagnostics - Periodic Event

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N08.FR.02 | ✅     |        |
| N08.FR.03 | ✅     |        |
| N08.FR.04 | ✅     |        |
| N08.FR.05 | ✅     |        |
| N08.FR.06 | ✅     |        |
| N08.FR.07 | ✅     |        |

## Diagnostics - Get Customer Information

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N09.FR.01 | ❎     |        |
| N09.FR.02 | ✅     |        |
| N09.FR.03 | ✅     |        |
| N09.FR.04 | ❎     |        |
| N09.FR.05 | ✅     |        |
| N09.FR.06 | ✅     |        |
| N09.FR.07 | ✅     |        |
| N09.FR.08 | ❎     |        |
| N09.FR.09 |        |        |

## Diagnostics - Clear Customer Information

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N10.FR.01 | ✅     |        |
| N10.FR.02 | ❎     |        |
| N10.FR.03 | ✅     |        |
| N10.FR.04 | ✅     |        |
| N10.FR.05 | ✅     |        |
| N10.FR.06 | ✅     |        |
| N10.FR.07 | ✅     |        |
| N10.FR.08 | ❎     |        |
| N10.FR.09 | ❎     |        |

## Diagnostics - Set Frequent Periodic Variable Monitoring (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N11.FR.01 |        |        |
| N11.FR.02 |        |        |
| N11.FR.03 |        |        |
| N11.FR.04 |        |        |
| N11.FR.05 |        |        |
| N11.FR.06 |        |        |
| N11.FR.07 |        |        |
| N11.FR.08 |        |        |
| N11.FR.09 |        |        |

## Diagnostics - Get Periodic Event Streams (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N12.FR.01 |        |        |

## Diagnostics - Close Periodic Event Streams (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N13.FR.01 |        |        |
| N13.FR.02 |        |        |
| N13.FR.03 |        |        |
| N13.FR.04 |        |        |
| N13.FR.05 |        |        |
| N13.FR.06 |        |        |

## Diagnostics - Adjust Periodic Event Streams (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N14.FR.01 |        |        |
| N14.FR.02 |        |        |
| N14.FR.03 |        |        |

## Diagnostics - Periodic Event Stream (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| N15.FR.01 |        |        |
| N15.FR.02 |        |        |
| N15.FR.03 |        |        |
| N15.FR.04 |        |        |
| N15.FR.05 |        |        |
| N15.FR.06 |        |        |
| N15.FR.07 |        |        |
| N15.FR.08 |        |        |
| N15.FR.09 |        |        |

## DisplayMessage - Set DisplayMessage

| ID                   | Status   | Remark |
| -------------------- | -------- | ------ |
| O01.FR.01            | ✅       |        |
| O01.FR.02            | ✅       |        |
| O01.FR.03            | ✅       |        |
| O01.FR.04            | 🌐       |        |
| O01.FR.05            | 🌐       |        |
| O01.FR.06            | ⛽️      |        |
| O01.FR.07            | ⛽️      |        |
| O01.FR.08            | ⛽️      |        |
| O01.FR.09            | ⛽️      |        |
| O01.FR.10            | ⛽️      |        |
| O01.FR.11            | ⛽️      |        |
| O01.FR.12            | ⛽️      |        |
| O01.FR.13            | ⛽️      |        |
| O01.FR.14            | ⛽️      |        |
| O01.FR.15            | ⛽️      |        |
| O01.FR.16            | ⛽️      |        |
| O01.FR.17            | ⛽️ / 🌐 |        |
| O01.FR.18 <br> (2.1) |          |        |
| O01.FR.19 <br> (2.1) |          |        |

## DisplayMessage - Set DisplayMessage for Transaction

| ID        | Status   | Remark |
| --------- | -------- | ------ |
| O02.FR.01 | ✅       |        |
| O02.FR.02 | ⛽️      |        |
| O02.FR.03 | ✅       |        |
| O02.FR.04 | ✅       |        |
| O02.FR.05 | ✅       |        |
| O02.FR.06 | ⛽️      |        |
| O02.FR.07 | ⛽️      |        |
| O02.FR.08 | ⛽️      |        |
| O02.FR.09 | ⛽️      |        |
| O02.FR.10 | ⛽️      |        |
| O02.FR.11 | ⛽️      |        |
| O02.FR.12 | ⛽️ / 🌐 |        |
| O02.FR.14 | ⛽️      |        |
| O02.FR.15 | ⛽️      |        |
| O02.FR.16 | ⛽️      |        |
| O02.FR.17 | ⛽️      |        |
| O02.FR.18 | ⛽️      |        |

## DisplayMessage - Get All DisplayMessages

| ID        | Status | Remark |
| --------- | ------ | ------ |
| O03.FR.01 | ✅     |        |
| O03.FR.02 | ✅     |        |
| O03.FR.03 |        |        |
| O03.FR.04 |        |        |
| O03.FR.05 |        |        |
| O03.FR.06 | ✅     |        |

## DisplayMessage - Get Specific DisplayMessages

| ID        | Status | Remark |
| --------- | ------ | ------ |
| O04.FR.01 | ✅     |        |
| O04.FR.02 | ✅     |        |
| O04.FR.03 | ✅     |        |
| O04.FR.04 |        |        |
| O04.FR.05 |        |        |
| O04.FR.06 |        |        |
| O04.FR.07 | ✅     |        |

## DisplayMessage - Clear a DisplayMessage

| ID                   | Status | Remark |
| -------------------- | ------ | ------ |
| O05.FR.01            | ⛽️    |        |
| O05.FR.02            | ⛽️    |        |
| O05.FR.03 <br> (2.1) |        |        |

## DisplayMessage - Replace DisplayMessage

| ID        | Status | Remark |
| --------- | ------ | ------ |
| O06.FR.01 | ⛽️    |        |

## DataTransfer - Data Transfer to the Charging Station

| ID        | Status | Remark                                                    |
| --------- | ------ | --------------------------------------------------------- |
| P01.FR.01 | ✅     | There is no way yet to register a data transfer callback. |
| P01.FR.02 | ❎     |                                                           |
| P01.FR.03 | ❎     |                                                           |
| P01.FR.04 | ❎     |                                                           |
| P01.FR.05 | ✅     |                                                           |
| P01.FR.06 | ✅     |                                                           |
| P01.FR.07 | ❎     |                                                           |

## DataTransfer - Data Transfer to the CSMS

| ID        | Status | Remark |
| --------- | ------ | ------ |
| P02.FR.01 | ⛽️    |        |
| P02.FR.02 | ⛽️    |        |
| P02.FR.03 | ❎     |        |
| P02.FR.04 | ⛽️    |        |
| P02.FR.05 | ❎     |        |
| P02.FR.06 | ❎     |        |
| P02.FR.07 | ❎     |        |
| P02.FR.08 | ❎     |        |

## Bidirectional Power Transfer - Generic smart charging rules for V2X (New in OCPP 2.1)

| ID     | Status | Remark |
| ------ | ------ | ------ |
| V2X.01 |        |        |
| V2X.02 |        |        |
| V2X.03 |        |        |
| V2X.04 |        |        |
| V2X.05 |        |        |

## Bidirectional Power Transfer - V2X Authorization (New in OCPP 2.1)

| ID        | Status                                          | Remark |
| --------- | ----------------------------------------------- | ------ |
|           | AllowedEnergyTransfer / RequestedEnergyTransfer |        |
| Q01.FR.01 |   🌐                                            |        |
| Q01.FR.02 |   ⛽️                                            | This is handled in OCPP2 module in EVerest. |
| Q01.FR.03 |   ✅                                            | The consumer of libocpp shall call `on_ev_charging_needs` when ChargeParameterDiscoveryReq is received via ISO15118 |
| Q01.FR.04 |   🌐                                            |        |
| Q01.FR.05 |                                                 | In principle it is a charging station requirement, but should never occur / is not really possible to renegotiate at this time.       |
| Q01.FR.06 |   ✅                                            |        |
| Q01.FR.07 |   🌐                                            |        |
| Q01.FR.08 |   🌐                                            |        |
| Q01.FR.09 |   ✅                                            | The consumer of libocpp shall call `on_ev_charging_needs` when ChargeParameterDiscoveryReq is received via ISO15118 |
|           | Device model                                    | User should fill DM. |
| Q01.FR.30 |   ⛽️                                            | User should fill DM. |
| Q01.FR.31 |   ⛽️                                            | User should fill DM. |
| Q01.FR.32 |   ⛽️                                            | User should fill DM. |
| Q01.FR.33 |                                                 |        |
| Q01.FR.34 |                                                 |        |
| Q01.FR.35 |                                                 |        |
| Q01.FR.36 |   ⛽️                                            | User should fill DM. |

## Bidirectional Power Transfer - Charging only (V2X control) before starting V2X (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| Q02.FR.01 |   🌐   |        |
| Q02.FR.02 |   🌐   |        |
| Q02.FR.03 |   🌐   |        |
| Q02.FR.04 |   ✅   | The consumer of libocpp shall call `on_ev_charging_needs` when ChargeParameterDiscoveryReq is received via ISO15118       |
| Q02.FR.05 |   🌐   |        |
| Q02.FR.06 |   ✅   |        |
| Q02.FR.07 |   ✅   | The consumer of libocpp shall call `on_ev_charging_needs` when ChargeParameterDiscoveryReq is received via ISO15118       |

## Bidirectional Power Transfer - Charging only (V2X control) before starting V2X (New in OCPP 2.1)

| ID        | Status                        | Remark |
| --------- | ----------------------------- | ------ |
|           | OperationMode CentralSetpoint |        |
| Q03.FR.01 |                               |        |
| Q03.FR.02 |                               |        |

## Bidirectional Power Transfer - External V2X setpoint control with a charging profile from CSMS (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| Q05.FR.01 |        |        |
| Q05.FR.02 |        |        |
| Q05.FR.03 |        |        |
| Q05.FR.04 |        |        |
| Q05.FR.05 |        |        |
| Q05.FR.06 |        |        |
| Q05.FR.07 |        |        |

## Bidirectional Power Transfer - External V2X control with a charging profile from an External System (New in OCPP 2.1)

| ID        | Status              | Remark |
| --------- | ------------------- | ------ |
| Q06.FR.01 |                     |        |
| Q06.FR.02 |                     |        |
| Q06.FR.03 |                     |        |
| Q06.FR.04 |                     |        |
| Q06.FR.05 |                     |        |
| Q06.FR.06 |                     |        |
| Q06.FR.07 |                     |        |
| Q06.FR.08 |                     |        |
|           | NotifyChargingLimit |        |
| Q06.FR.10 |                     |        |
| Q06.FR.11 |                     |        |
| Q06.FR.12 |                     |        |
|           | SetpointPriority    |        |
| Q06.FR.20 |                     |        |
| Q06.FR.21 |                     |        |
| Q06.FR.22 |                     |        |
|           | evseId = 0          |        |
| Q06.FR.30 |                     |        |
| Q06.FR.31 |                     |        |

## Bidirectional Power Transfer - Central V2X control for frequency support (New in OCPP 2.1)

This use case adheres to requirements related to CentralSetpoint from Q04 - Central V2X control with dynamic
CSMS setpoint and Dynamic charging profiles from K01. There are no specific requirements for this use case.

## Bidirectional Power Transfer - Local V2X control for frequency support (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
|           | FCR    |        |
| Q08.FR.01 |        |        |
| Q08.FR.02 | ✅      |        |
| Q08.FR.03 |        |        |
| Q08.FR.04 | ✅      |        |
| Q08.FR.05 | ✅      |        |
| Q08.FR.06 |        |        |
| Q08.FR.07 |        |        |
|           | aFRR   |        |
| Q08.FR.10 |        |        |
| Q08.FR.11 |        |        |

## Bidirectional Power Transfer - Local V2X control for load balancing (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| Q09.FR.01 |        |        |
| Q09.FR.02 |        |        |
| Q09.FR.03 |        |        |
| Q09.FR.04 |        |        |
| Q09.FR.05 |        |        |

## Bidirectional Power Transfer - Idle, minimizing energy consumption (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| Q10.FR.01 | ✅      |        |
| Q10.FR.02 | ✅      |        |
| Q10.FR.03 |        |        |
| Q10.FR.04 |        |        |
| Q10.FR.05 |        |        |

## Bidirectional Power Transfer - Going offline during V2X operation

| ID  | Status | Remark |
| --- | ------ | ------ |
| Q11 | ✅      |Use Case doesn't contain any functional requirement|

## Bidirectional Power Transfer - Resuming a V2X operation after an offline period

| ID  | Status | Remark |
| --- | ------ | ------ |
| Q12 | ✅      | Use Case doesn't contain any functional requirement|

## DER Control - Starting a V2X session with DER control in EVSE (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| R01.FR.01 |        |        |
| R01.FR.02 |        |        |
| R01.FR.03 |        |        |

## DER Control - Starting a V2X session with DER control in EV (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| R02.FR.01 |        |        |
| R02.FR.02 |        |        |
| R02.FR.03 |        |        |

## DER Control - Starting a V2X session with hybrid DER control in both EV and EVSE (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| R03.FR.01 |        |        |
| R03.FR.02 |        |        |
| R03.FR.03 |        |        |
| R03.FR.04 |        |        |
| R03.FR.05 |        |        |
| R03.FR.06 |        |        |

## DER Control - Configure DER control settings at Charging Station (New in OCPP 2.1)

| ID        | Status             | Remark |
| --------- | ------------------ | ------ |
|           | SetDERControl      |        |
| R04.FR.01 |                    |        |
| R04.FR.02 |                    |        |
| R04.FR.03 |                    |        |
| R04.FR.04 |                    |        |
| R04.FR.05 |                    |        |
| R04.FR.06 |                    |        |
| R04.FR.07 |                    |        |
| R04.FR.08 |                    |        |
| R04.FR.09 |                    |        |
| R04.FR.10 |                    |        |
| R04.FR.11 |                    |        |
|           | NotifyDERStartStop |        |
| R04.FR.20 |                    |        |
| R04.FR.21 |                    |        |
| R04.FR.23 |                    |        |
|           | GetDERControl      |        |
| R04.FR.30 |                    |        |
| R04.FR.31 |                    |        |
| R04.FR.32 |                    |        |
| R04.FR.33 |                    |        |
| R04.FR.34 |                    |        |
| R04.FR.35 |                    |        |
| R04.FR.36 |                    |        |
|           | ClearDERControl    |        |
| R04.FR.40 |                    |        |
| R04.FR.41 |                    |        |
| R04.FR.42 |                    |        |
| R04.FR.43 |                    |        |
| R04.FR.44 |                    |        |
| R04.FR.45 |                    |        |

## DER Control - Charging station reporting a DER event (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| R05.FR.01 |        |        |
| R05.FR.02 |        |        |
| R05.FR.03 |        |        |
| R05.FR.04 |        |        |

## Battery Swapping - Battery Swap Local Authorization (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| S01.FR.01 |        |        |
| S01.FR.02 |        |        |

## Battery Swapping - Battery Remote Start (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| S02.FR.01 |        |        |
| S02.FR.02 |        |        |
| S02.FR.03 |        |        |
| S02.FR.04 |        |        |
| S02.FR.05 |        |        |

## Battery Swapping - Battery Swap In/Out (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| S03.FR.01 |        |        |
| S03.FR.02 |        |        |
| S03.FR.03 |        |        |
| S03.FR.04 |        |        |
| S03.FR.05 |        |        |
| S03.FR.06 |        |        |

## Battery Swapping - Battery Swap Charging (New in OCPP 2.1)

| ID        | Status | Remark |
| --------- | ------ | ------ |
| S04.FR.01 |        |        |
| S04.FR.02 |        |        |
| S04.FR.03 |        |        |
| S04.FR.04 |        |        |
| S04.FR.05 |        |        |
| S04.FR.06 |        |        |
| S04.FR.07 |        |        |
| S04.FR.08 |        |        |
| S04.FR.09 |        |        |
| S04.FR.10 |        |        |
| S04.FR.11 |        |        |
