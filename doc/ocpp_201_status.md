This document contains the status of which OCPP 2.0.1 numbered functional requirements (FRs) have been implemented in `libocpp`. This does not cover if the functionality is also implemented in `everest-core`.

## Legend

| Status | Description                                                                    |
|--------|--------------------------------------------------------------------------------|
| ✅     | Satisfied                                                                      |
| ❎     | Not applicable                                                                 |
| ⛽️     | A functional requirement for other systems in the Charging Station             |
| 🌐     | A functional requirement for the CSMS                                          |
| 💂     | Improper behavior by another actor is guarded against                          |
| ❓     | Actor responsible for or status of requirement is unknown                      |
| 🤓     | Catch-all for FRs that are satisfied for other reasons (see the Remark column) |


## General - General

| ID    | Status | Remark |
|-------|--------|--------|
| FR.01 | ✅     |        |
| FR.02 | ✅     |        |
| FR.03 | ✅     |        |
| FR.04 | ❎     |        |
| FR.05 | ✅     |        |

## Security - Generic Security Profile requirements

| ID         | Status | Remark |
|------------|--------|--------|
| A00.FR.001 | ✅     |        |
| A00.FR.002 | ✅     |        |
| A00.FR.003 | ✅     |        |
| A00.FR.004 | ✅     |        |
| A00.FR.005 | ✅     |        |
| A00.FR.006 | ✅     |        |

## Security - Unsecured Transport with Basic Authentication Profile

| ID         | Status | Remark |
|------------|--------|--------|
| A00.FR.201 | ❎     |        |
| A00.FR.202 | ✅     |        |
| A00.FR.203 | ✅     |        |
| A00.FR.204 | ✅     |        |
| A00.FR.205 | ✅     |        |
| A00.FR.206 | ✅     |        |
| A00.FR.207 | ❎     |        |

## Security - TLS with Basic Authentication Profile

| ID         | Status | Remark          |
|------------|--------|-----------------|
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

| ID         | Status | Remark |
|------------|--------|--------|
| A00.FR.401 | ✅     |        |
| A00.FR.402 | ✅     |        |
| A00.FR.403 | ❎     |        |
| A00.FR.404 | ❎     |        |
| A00.FR.405 | ❎     |        |
| A00.FR.406 | ❎     |        |
| A00.FR.407 | ❎     |        |
| A00.FR.408 | ❎     |        |
| A00.FR.409 | ❎     |        |
| A00.FR.410 | ❎     |        |
| A00.FR.411 | ✅     |        |
| A00.FR.412 | ✅     |        |
| A00.FR.413 |        |        |
| A00.FR.414 | ✅     |        |
| A00.FR.415 | ✅     |        |
| A00.FR.416 | ✅     |        |
| A00.FR.417 | ✅     |        |
| A00.FR.418 | ❎     |        |
| A00.FR.419 |        |        |
| A00.FR.420 | ✅     |        |
| A00.FR.421 | ❎     |        |
| A00.FR.422 | ✅     |        |
| A00.FR.423 | ✅     |        |
| A00.FR.424 | ✅     |        |
| A00.FR.425 | ❎     |        |
| A00.FR.426 |        |        |
| A00.FR.427 | ❎     |        |
| A00.FR.428 | ❎     |        |
| A00.FR.429 | ❎     |        |

## Security - Certificate Properties

| ID         | Status | Remark |
|------------|--------|--------|
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
|------------|--------|--------|
| A00.FR.601 | ❎     |        |
| A00.FR.602 | ❎     |        |
| A00.FR.603 | ❎     |        |
| A00.FR.604 | ✅     |        |

## Security - Certificate Revocation

| ID         | Status | Remark |
|------------|--------|--------|
| A00.FR.701 | ❎     |        |
| A00.FR.702 | ❎     |        |
| A00.FR.703 | ❎     |        |
| A00.FR.704 | ❎     |        |
| A00.FR.705 | ❎     |        |
| A00.FR.707 | ❎     |        |

## Security - Installation

| ID         | Status | Remark |
|------------|--------|--------|
| A00.FR.801 | ❎     |        |
| A00.FR.802 | ❎     |        |
| A00.FR.803 | ❎     |        |
| A00.FR.804 | ❎     |        |
| A00.FR.805 | ❎     |        |
| A00.FR.806 | ❎     |        |
| A00.FR.807 | ❎     |        |

## Security - Update Charging Station Password for HTTP Basic Authentication

| ID        | Status | Remark |
|-----------|--------|--------|
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

| ID        | Status | Remark                                                           |
|-----------|--------|------------------------------------------------------------------|
| A02.FR.01 | ❎     |                                                                  |
| A02.FR.02 | ✅     |                                                                  |
| A02.FR.03 | ✅     |                                                                  |
| A02.FR.04 | ❎     |                                                                  |
| A02.FR.05 | ✅     |                                                                  |
| A02.FR.06 | ✅     |                                                                  |
| A02.FR.07 | ✅     |                                                                  |
| A02.FR.08 |        | This is done on next use of cert if cert is valid in the future. |
| A02.FR.09 | ✅     |                                                                  |
| A02.FR.10 | ❎     |                                                                  |
| A02.FR.11 | ❎     |                                                                  |
| A02.FR.12 | ❎     |                                                                  |
| A02.FR.13 | ✅     |                                                                  |
| A02.FR.14 | ❎     |                                                                  |
| A02.FR.15 | ✅     |                                                                  |
| A02.FR.16 |        |                                                                  |
| A02.FR.17 | ✅     |                                                                  |
| A02.FR.18 | ✅     |                                                                  |
| A02.FR.19 | ✅     |                                                                  |
| A02.FR.20 | ✅     |                                                                  |
| A02.FR.21 |        |                                                                  |

## Security - Update Charging Station Certificate initiated by the Charging Station

| ID        | Status | Remark |
|-----------|--------|--------|
| A03.FR.01 | ❎     |        |
| A03.FR.02 | ✅     |        |
| A03.FR.03 | ✅     |        |
| A03.FR.04 | ❎     |        |
| A03.FR.05 | ✅     |        |
| A03.FR.06 | ✅     |        |
| A03.FR.07 | ✅     |        |
| A03.FR.08 |        |        |
| A03.FR.09 | ✅     |        |
| A03.FR.10 | ❎     |        |
| A03.FR.11 | ❎     |        |
| A03.FR.12 | ❎     |        |
| A03.FR.13 | ✅     |        |
| A03.FR.14 | ❎     |        |
| A03.FR.15 | ✅     |        |
| A03.FR.16 |        |        |
| A03.FR.17 | ✅     |        |
| A03.FR.18 | ✅     |        |
| A03.FR.19 | ✅     |        |

## Security - Security Event Notification

| ID        | Status | Remark |
|-----------|--------|--------|
| A04.FR.01 | ✅     |        |
| A04.FR.02 | ✅     |        |
| A04.FR.03 | ❎     |        |
| A04.FR.04 | ✅     |        |

## Security - Upgrade Charging Station Security Profile

| ID        | Status | Remark |
|-----------|--------|--------|
| A05.FR.02 | ✅     |        |
| A05.FR.03 | ✅     |        |
| A05.FR.04 | ✅     |        |
| A05.FR.05 | ✅     |        |
| A05.FR.06 |        |        |
| A05.FR.07 | ❎     |        |

## Provisioning - Cold Boot Charging Station

| ID        | Status | Remark |
|-----------|--------|--------|
| B01.FR.01 | ✅     |        |
| B01.FR.02 | ❎     |        |
| B01.FR.03 | ✅     |        |
| B01.FR.04 | ✅     |        |
| B01.FR.05 | ✅     |        |
| B01.FR.06 | ❎     |        |
| B01.FR.07 | ✅     |        |
| B01.FR.08 | ✅     |        |
| B01.FR.09 | ✅     |        |
| B01.FR.10 | ❎     |        |
| B01.FR.11 | ❎     |        |
| B01.FR.12 | ❎     |        |
| B01.FR.13 |        |        |

## Provisioning - Cold Boot Charging Station – Pending

| ID        | Status | Remark                                           |
|-----------|--------|--------------------------------------------------|
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
|-----------|--------|--------|
| B03.FR.01 | ✅     |        |
| B03.FR.02 | ✅     |        |
| B03.FR.03 | ❎     |        |
| B03.FR.04 | ✅     |        |
| B03.FR.05 | ✅     |        |
| B03.FR.06 | ✅     |        |
| B03.FR.07 | ❎     |        |
| B03.FR.08 | ✅     |        |

## Provisioning - Offline Behavior Idle Charging Station

| ID        | Status | Remark |
|-----------|--------|--------|
| B04.FR.01 | ✅     |        |
| B04.FR.02 | ✅     |        |

## Provisioning - Set Variables

| ID        | Status | Remark |
|-----------|--------|--------|
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
|-----------|--------|--------|
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

| ID        | Status | Remark                    |
|-----------|--------|---------------------------|
| B07.FR.01 | ✅     |                           |
| B07.FR.02 | ✅     |                           |
| B07.FR.03 | ✅     |                           |
| B07.FR.04 | ✅     |                           |
| B07.FR.05 | ✅     |                           |
| B07.FR.06 | ✅     |                           |
| B07.FR.07 | ✅     |                           |
| B07.FR.08 | ✅     |                           |
| B07.FR.09 | ✅     |                           |
| B07.FR.10 | ✅     |                           |
| B07.FR.11 | ✅     |                           |
| B07.FR.12 | ✅     |                           |
| B07.FR.13 | ❎     | tbd if this is applicable |
| B07.FR.14 | ❎     |                           |

## Provisioning - Get Custom Report

| ID        | Status | Remark |
|-----------|--------|--------|
| B08.FR.01 | ✅     |        |
| B08.FR.02 | ✅     |        |
| B08.FR.03 | ✅     |        |
| B08.FR.04 | ✅     |        |
| B08.FR.05 | ✅     |        |
| B08.FR.06 | ❎     |        |
| B08.FR.07 | ✅     |        |
| B08.FR.08 | ✅     |        |
| B08.FR.09 | ✅     |        |
| B08.FR.10 | ✅     |        |
| B08.FR.11 | ✅     |        |
| B08.FR.12 | ✅     |        |
| B08.FR.13 | ✅     |        |
| B08.FR.14 | ✅     |        |
| B08.FR.15 | ✅     |        |
| B08.FR.16 | ✅     |        |
| B08.FR.17 | ✅     |        |
| B08.FR.18 | ✅     |        |
| B08.FR.19 |        |        |
| B08.FR.20 |        |        |
| B08.FR.21 |        |        |

## Provisioning - Setting a new NetworkConnectionProfile

| ID        | Status | Remark |
|-----------|--------|--------|
| B09.FR.01 | ✅     |        |
| B09.FR.02 | ✅     |        |
| B09.FR.03 | ✅     |        |
| B09.FR.04 | ✅     |        |

## Provisioning - Migrate to new CSMS

| ID        | Status | Remark                                                      |
|-----------|--------|-------------------------------------------------------------|
| B10.FR.01 | ✅     |                                                             |
| B10.FR.02 | ✅     |                                                             |
| B10.FR.03 | ✅     |                                                             |
| B10.FR.04 | ✅     |                                                             |
| B10.FR.05 |        |                                                             |
| B10.FR.06 | ✅     |                                                             |
| B10.FR.07 | ✅     | tbd. we're looping over priorities and attempt to reconnect |

## Provisioning - Reset - Without Ongoing Transaction

| ID        | Status | Remark                                        |
|-----------|--------|-----------------------------------------------|
| B11.FR.01 | ✅     |                                               |
| B11.FR.02 | ✅     |                                               |
| B11.FR.03 | ✅     |                                               |
| B11.FR.04 | ✅     |                                               |
| B11.FR.05 |        |                                               |
| B11.FR.06 | ⛽️     | In EVerest, the System module is responsible. |
| B11.FR.07 | ⛽️     | In EVerest, the System module is responsible. |
| B11.FR.08 | ✅     |                                               |
| B11.FR.09 | ✅     |                                               |
| B11.FR.10 | ✅     | has to be set in device model                 |

## Provisioning - Reset - With Ongoing Transaction

| ID        | Status | Remark                                                                           |
|-----------|--------|----------------------------------------------------------------------------------|
| B12.FR.01 | ✅     |                                                                                  |
| B12.FR.02 | ✅     |                                                                                  |
| B12.FR.03 | ✅     |                                                                                  |
| B12.FR.04 | ✅     |                                                                                  |
| B12.FR.05 | ✅     |                                                                                  |
| B12.FR.06 | ⛽️     | Charging station is responsible to send the correct state after booting          |
| B12.FR.07 | ✅     |                                                                                  |
| B12.FR.08 | ✅     |                                                                                  |
| B12.FR.09 | ⛽️     | Charging Station should respond with a "rejected" on `is_reset_allowed_callback` |

## Authorization - EV Driver Authorization using RFID

| ID        | Status | Remark                                           |
|-----------|--------|--------------------------------------------------|
| C01.FR.01 | ✅     |                                                  |
| C01.FR.02 | ✅     |                                                  |
| C01.FR.03 | ✅     |                                                  |
| C01.FR.04 | ✅     |                                                  |
| C01.FR.05 | ✅     |                                                  |
| C01.FR.06 | ✅     |                                                  |
| C01.FR.07 | ✅     |                                                  |
| C01.FR.08 |        | This to FR.17 are all language related usecases. |
| C01.FR.09 |        |                                                  |
| C01.FR.10 |        |                                                  |
| C01.FR.11 |        |                                                  |
| C01.FR.12 |        |                                                  |
| C01.FR.13 |        |                                                  |
| C01.FR.17 |        |                                                  |
| C01.FR.18 | ✅     |                                                  |
| C01.FR.19 | ✅     |                                                  |
| C01.FR.20 | ✅     |                                                  |
| C01.FR.21 | ✅     | Auth mechanism is responsible.                   |
| C01.FR.22 | ✅     |                                                  |
| C01.FR.23 | ✅     |                                                  |
| C01.FR.24 | ✅     |                                                  |

## Authorization - Authorization using a start button

| ID        | Status | Remark |
|-----------|--------|--------|
| C02.FR.01 | ❎     |        |
| C02.FR.02 | ❎     |        |
| C02.FR.03 |        |        |

## Authorization - Authorization using credit/debit card

| ID        | Status | Remark |
|-----------|--------|--------|
| C03.FR.01 | ✅     |        |
| C03.FR.02 | ✅     |        |

## Authorization - Authorization using PIN-code

| ID        | Status | Remark |
|-----------|--------|--------|
| C04.FR.01 | ❎     |        |
| C04.FR.02 | ❎     |        |
| C04.FR.03 | ❎     |        |
| C04.FR.04 | ❎     |        |
| C04.FR.05 | ❎     |        |
| C04.FR.06 | ❎     |        |

## Authorization - Authorization for CSMS initiated transactions

| ID        | Status | Remark |
|-----------|--------|--------|
| C05.FR.01 | ✅     |        |
| C05.FR.02 | ✅     |        |
| C05.FR.03 | ⛽️     |        |
| C05.FR.04 |        |        |
| C05.FR.05 | ✅     |        |

## Authorization - Authorization using local id type

| ID        | Status | Remark |
|-----------|--------|--------|
| C06.FR.01 | ✅     |        |
| C06.FR.02 | ✅     |        |
| C06.FR.03 | ✅     |        |
| C06.FR.04 | ❎     |        |

## Authorization - Authorization using Contract Certificates

| ID        | Status | Remark |
|-----------|--------|--------|
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

## Authorization - Authorization at EVSE using ISO 15118 External Identification Means (EIM)

| ID        | Status | Remark |
|-----------|--------|--------|
| C08.FR.01 |        |        |
| C08.FR.02 |        |        |

## Authorization - Authorization by GroupId

| ID        | Status | Remark |
|-----------|--------|--------|
| C09.FR.02 | ❎     |        |
| C09.FR.03 | ✅     |        |
| C09.FR.04 | ✅     |        |
| C09.FR.05 | ✅     |        |
| C09.FR.07 | ✅     |        |
| C09.FR.09 | ❎     |        |
| C09.FR.10 | ❎     |        |
| C09.FR.11 | ✅     |        |
| C09.FR.12 | ❎     |        |

## Authorization - Store Authorization Data in the Authorization Cache

| ID        | Status | Remark      |
|-----------|--------|-------------|
| C10.FR.01 | ✅     |             |
| C10.FR.02 | ✅     |             |
| C10.FR.03 | ✅     |             |
| C10.FR.04 | ✅     |             |
| C10.FR.05 | ✅     |             |
| C10.FR.06 |        | Reservation |
| C10.FR.07 | ✅     | deferred    |
| C10.FR.08 | ✅     |             |
| C10.FR.09 |        | deferred    |
| C10.FR.10 | ✅     |             |
| C10.FR.11 | ✅     |             |
| C10.FR.12 | ✅     |             |

## Authorization - Clear Authorization Data in Authorization Cache

| ID        | Status | Remark |
|-----------|--------|--------|
| C11.FR.01 | ✅     |        |
| C11.FR.02 | ✅     |        |
| C11.FR.03 | ✅     |        |
| C11.FR.04 | ✅     |        |
| C11.FR.05 | ✅     |        |

## Authorization - Start Transaction - Cached Id

| ID        | Status | Remark                                      |
|-----------|--------|---------------------------------------------|
| C12.FR.02 | ✅     |                                             |
| C12.FR.03 | ✅     |                                             |
| C12.FR.04 | ✅     |                                             |
| C12.FR.05 | ✅     |                                             |
| C12.FR.06 | ✅     |                                             |
| C12.FR.09 | ⛽️     | In EVerest, the Auth module is responsible. |

## Authorization - Offline Authorization through Local Authorization List

| ID        | Status | Remark |
|-----------|--------|--------|
| C13.FR.01 | ✅     |        |
| C13.FR.02 | ✅     |        |
| C13.FR.03 | ✅     |        |
| C13.FR.04 | ✅     |        |

## Authorization - Online Authorization through Local Authorization List

| ID        | Status | Remark |
|-----------|--------|--------|
| C14.FR.01 | ✅     |        |
| C14.FR.02 | ✅     |        |
| C14.FR.03 | ✅     |        |

## Authorization - Offline Authorization of unknown Id

| ID        | Status | Remark |
|-----------|--------|--------|
| C15.FR.01 | ✅     |        |
| C15.FR.02 | ✅     |        |
| C15.FR.03 | ✅     |        |
| C15.FR.04 | ✅     |        |
| C15.FR.05 | ⛽️     |        |
| C15.FR.06 | ✅     |        |
| C15.FR.07 | ✅     |        |
| C15.FR.08 | ✅     |        |

## Authorization - Stop Transaction with a Master Pass

| ID        | Status | Remark        |
|-----------|--------|---------------|
| C16.FR.01 |        |               |
| C16.FR.02 | ⛽️     | Core changes? |
| C16.FR.03 | ⛽️     | Core changes  |
| C16.FR.04 |        |               |
| C16.FR.05 |        |               |

## LocalAuthorizationListManagement - Send Local Authorization List

| ID        | Status | Remark |
|-----------|--------|--------|
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
|-----------|--------|--------|
| D02.FR.01 | ✅     |        |
| D02.FR.02 | ✅     |        |
| D02.FR.03 | ✅     |        |

## Transactions - Start Transaction Options

| ID        | Status | Remark |
|-----------|--------|--------|
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

| ID        | Status | Remark |
|-----------|--------|--------|
| E02.FR.01 | ✅     |        |
| E02.FR.02 | ✅     |        |
| E02.FR.03 |        |        |
| E02.FR.04 | ✅     |        |
| E02.FR.05 | ✅     |        |
| E02.FR.06 | ❎     |        |
| E02.FR.07 | ✅     |        |
| E02.FR.08 | ✅     |        |
| E02.FR.09 | ✅     |        |
| E02.FR.10 | ✅     |        |
| E02.FR.11 | ❎     | tbd    |
| E02.FR.13 | ✅     |        |
| E02.FR.14 | ✅     |        |
| E02.FR.15 | ✅     |        |
| E02.FR.16 | ✅     |        |
| E02.FR.17 | ✅     |        |
| E02.FR.18 |        |        |
| E02.FR.19 |        |        |
| E02.FR.20 | ✅     |        |
| E02.FR.21 | ✅     |        |

## Transactions - Start Transaction - IdToken First

| ID        | Status | Remark |
|-----------|--------|--------|
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
|-----------|--------|--------|
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

| ID        | Status | Remark |
|-----------|--------|--------|
| E05.FR.01 | ✅     |        |
| E05.FR.02 | ✅     |        |
| E05.FR.03 | ✅     |        |
| E05.FR.04 | ✅     |        |
| E05.FR.05 | ✅     |        |
| E05.FR.06 | ✅     |        |
| E05.FR.08 | ✅     |        |
| E05.FR.09 |        |        |
| E05.FR.10 | ✅     |        |
| E05.FR.11 | ❎     |        |

## Transactions - Stop Transaction options

| ID        | Status | Remark |
|-----------|--------|--------|
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
|-----------|--------|--------|
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
|-----------|--------|--------|
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

| ID        | Status | Remark                                                         |
|-----------|--------|----------------------------------------------------------------|
| E09.FR.01 | ✅     | `StopTxOnEVSideDisconnect` is RO for our implementation so far |
| E09.FR.02 |        |                                                                |
| E09.FR.03 |        |                                                                |
| E09.FR.04 | ✅     |                                                                |
| E09.FR.05 | ✅     |                                                                |
| E09.FR.06 | ✅     |                                                                |
| E09.FR.07 | ✅     |                                                                |

## Transactions -  When cable disconnected on EV-side: Suspend Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| E10.FR.01 |        |        |

## Transactions - When cable disconnected on EV-side: Stop Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| E10.FR.02 | ✅     |        |
| E10.FR.03 | ✅     |        |
| E10.FR.04 | ✅     |        |
| E10.FR.05 | ❎     | tbd    |
| E10.FR.06 |        | tbd    |
| E10.FR.07 | ✅     | tbd    |

## Transactions - Connection Loss During Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| E11.FR.01 | ✅     |        |
| E11.FR.02 | ✅     |        |
| E11.FR.03 | ✅     |        |
| E11.FR.04 | ✅     |        |
| E11.FR.05 | ✅     |        |
| E11.FR.06 | ✅     |        |
| E11.FR.07 | ✅     |        |
| E11.FR.08 | ✅     |        |

## Transactions - Inform CSMS of an Offline Occurred Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| E12.FR.01 | ✅     |        |
| E12.FR.02 | ✅     |        |
| E12.FR.03 | ✅     |        |
| E12.FR.04 | ✅     |        |
| E12.FR.05 | ✅     |        |
| E12.FR.06 | ✅     |        |
| E12.FR.07 | ✅     |        |
| E12.FR.08 | ✅     |        |
| E12.FR.09 | ✅     |        |
| E12.FR.10 | ✅     |        |

## Transactions - Transaction-related message not accepted by CSMS

| ID        | Status | Remark |
|-----------|--------|--------|
| E13.FR.01 | ✅     |        |
| E13.FR.02 | ✅     |        |
| E13.FR.03 | ✅     |        |
| E13.FR.04 | ✅     |        |

## Transactions - Check transaction status

| ID        | Status | Remark |
|-----------|--------|--------|
| E14.FR.01 | ✅     |        |
| E14.FR.02 | ✅     |        |
| E14.FR.03 | ✅     |        |
| E14.FR.04 | ✅     |        |
| E14.FR.05 | ✅     |        |
| E14.FR.06 | ✅     |        |
| E14.FR.07 | ✅     |        |
| E14.FR.08 | ✅     |        |

## Transactions - End of charging process

| ID        | Status | Remark |
|-----------|--------|--------|
| E15.FR.01 | ✅     |        |
| E15.FR.02 | ❎     | tbd    |
| E15.FR.03 | ❎     | tbd    |
| E15.FR.04 | ✅     |        |

## RemoteControl - Remote Start Transaction - Cable Plugin First

| ID        | Status | Remark                                                   |
|-----------|--------|----------------------------------------------------------|
| F01.FR.01 | ⛽️     |                                                          |
| F01.FR.02 | ⛽️     |                                                          |
| F01.FR.03 | ⛽️     |                                                          |
| F01.FR.04 | ⛽️     |                                                          |
| F01.FR.05 | ⛽️     |                                                          |
| F01.FR.06 | ✅     |                                                          |
| F01.FR.07 | ✅     | Currently always rejected                                |
| F01.FR.08 |        |                                                          |
| F01.FR.09 |        |                                                          |
| F01.FR.10 |        |                                                          |
| F01.FR.11 |        |                                                          |
| F01.FR.12 |        |                                                          |
| F01.FR.13 | ⛽️     |                                                          |
| F01.FR.14 | ⛽️     |                                                          |
| F01.FR.15 | ⛽️     |                                                          |
| F01.FR.16 | ⛽️     |                                                          |
| F01.FR.17 | ⛽️     |                                                          |
| F01.FR.18 | ⛽️     |                                                          |
| F01.FR.19 | ⛽️     |                                                          |
| F01.FR.20 | ✅     | Currently when no EVSE ID is given, request is rejected. |
| F01.FR.21 | ✅     |                                                          |
| F01.FR.22 | ✅     |                                                          |
| F01.FR.23 | ✅     |                                                          |
| F01.FR.24 | ✅     |                                                          |
| F01.FR.25 | ⛽️     |                                                          |
| F01.FR.26 |        |                                                          |

## RemoteControl - Remote Start Transaction - Remote Start First

| ID        | Status | Remark                                                   |
|-----------|--------|----------------------------------------------------------|
| F02.FR.01 | ⛽️     |                                                          |
| F02.FR.02 | ⛽️     |                                                          |
| F02.FR.03 | ⛽️     |                                                          |
| F02.FR.04 | ⛽️     |                                                          |
| F02.FR.05 | ⛽️     |                                                          |
| F02.FR.06 | ⛽️     |                                                          |
| F02.FR.07 | ⛽️     |                                                          |
| F02.FR.08 | ⛽️     |                                                          |
| F02.FR.09 | ⛽️     |                                                          |
| F02.FR.10 | ⛽️     |                                                          |
| F02.FR.11 | ⛽️❓   | Charging station or libocpp?                             |
| F02.FR.12 | ⛽️     |                                                          |
| F02.FR.13 | ⛽️     |                                                          |
| F02.FR.14 | ✅     |                                                          |
| F02.FR.15 | ✅     | Currently always rejected                                |
| F02.FR.16 |        |                                                          |
| F02.FR.17 |        |                                                          |
| F02.FR.18 |        |                                                          |
| F02.FR.19 |        |                                                          |
| F02.FR.20 |        |                                                          |
| F02.FR.21 | ⛽️     |                                                          |
| F02.FR.22 | ✅     | Currently when no EVSE ID is given, request is rejected. |
| F02.FR.23 | ✅     |                                                          |
| F02.FR.24 | ✅     |                                                          |
| F02.FR.25 | ✅     |                                                          |
| F02.FR.26 | ✅     |                                                          |
| F02.FR.27 |        |                                                          |

## RemoteControl - Remote Stop Transaction

| ID        | Status | Remark                                                        |
|-----------|--------|---------------------------------------------------------------|
| F03.FR.01 | ✅     |                                                               |
| F03.FR.02 | ⛽️     | The Charging Station should send a `TransactionEventRequest`. |
| F03.FR.03 | ⛽️     |                                                               |
| F03.FR.04 | ⛽️     |                                                               |
| F03.FR.05 | ⛽️     |                                                               |
| F03.FR.06 | ⛽️     |                                                               |
| F03.FR.07 | ✅     |                                                               |
| F03.FR.08 | ✅     |                                                               |
| F03.FR.09 | ⛽️     |                                                               |

## RemoteControl - Remote Stop ISO 15118 Charging from CSMS

| ID        | Status | Remark |
|-----------|--------|--------|
| F04.FR.01 | ❎     |        |
| F04.FR.02 | ✅     |        |
| F04.FR.03 | ✅     |        |
| F04.FR.04 | ✅     |        |
| F04.FR.05 |        |        |
| F04.FR.06 |        |        |

## RemoteControl - Remotely Unlock Connector

| ID        | Status | Remark |
|-----------|--------|--------|
| F05.FR.01 | ✅     |        |
| F05.FR.02 | ✅     |        |
| F05.FR.03 | ✅     |        |
| F05.FR.04 | ⛽️     |        |
| F05.FR.05 | ⛽️     |        |
| F05.FR.06 | ⛽️     |        |

## RemoteControl - Trigger Message

| ID        | Status | Remark |
|-----------|--------|--------|
| F06.FR.01 | ❎     |        |
| F06.FR.02 | ❎     |        |
| F06.FR.03 | ✅     |        |
| F06.FR.04 | ✅     |        |
| F06.FR.05 | ✅     |        |
| F06.FR.06 | ✅     |        |
| F06.FR.07 | ✅     |        |
| F06.FR.08 | ✅     |        |
| F06.FR.09 | ✅     |        |
| F06.FR.10 | ✅     |        |
| F06.FR.11 | ✅     |        |
| F06.FR.12 | ✅     |        |
| F06.FR.13 | ✅     |        |
| F06.FR.14 | ✅     |        |
| F06.FR.15 | ✅     |        |
| F06.FR.16 | ✅     |        |
| F06.FR.17 | ✅     |        |

## Availability - Status Notification

| ID        | Status | Remark |
|-----------|--------|--------|
| G01.FR.01 | ✅     |        |
| G01.FR.02 | ⛽️❓   |        |
| G01.FR.03 | ✅     |        |
| G01.FR.04 | ✅     |        |
| G01.FR.05 | ✅     |        |
| G01.FR.06 |        |        |
| G01.FR.07 | ✅     |        |
| G01.FR.08 | ⛽️❓   |        |

## Availability - Heartbeat

| ID        | Status | Remark                                    |
|-----------|--------|-------------------------------------------|
| G02.FR.01 | ✅     |                                           |
| G02.FR.02 | ✅     |                                           |
| G02.FR.03 | ❎     |                                           |
| G02.FR.04 | ❎     |                                           |
| G02.FR.05 |        | Not mandatory, so we can leave like this. |
| G02.FR.06 | ✅     |                                           |
| G02.FR.07 |        |                                           |

## Availability - Change Availability EVSE/Connector

| ID        | Status | Remark |
|-----------|--------|--------|
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
|-----------|--------|--------|
| G04.FR.01 | ⛽️❓   |        |
| G04.FR.02 | ✅     |        |
| G04.FR.03 | ✅     |        |
| G04.FR.04 | ✅     |        |
| G04.FR.05 | ⛽️     |        |
| G04.FR.06 | ✅     |        |
| G04.FR.07 | ✅     |        |
| G04.FR.08 | ✅     |        |
| G04.FR.09 | ⛽️     |        |

## Availability - Lock Failure

| ID        | Status | Remark |
|-----------|--------|--------|
| G05.FR.01 | ⛽️❓   |        |
| G05.FR.02 | ⛽️❓   |        |
| G05.FR.03 | 🌐     |        |
| G05.FR.04 | ⛽️     |        |

## Reservation - Reservation

| ID        | Status | Remark |
|-----------|--------|--------|
| H01.FR.01 |        |        |
| H01.FR.02 |        |        |
| H01.FR.03 |        |        |
| H01.FR.04 |        |        |
| H01.FR.06 |        |        |
| H01.FR.07 |        |        |
| H01.FR.09 |        |        |
| H01.FR.11 |        |        |
| H01.FR.12 |        |        |
| H01.FR.14 |        |        |
| H01.FR.15 |        |        |
| H01.FR.16 |        |        |
| H01.FR.17 |        |        |
| H01.FR.18 |        |        |
| H01.FR.19 |        |        |
| H01.FR.20 |        |        |
| H01.FR.23 |        |        |
| H01.FR.24 |        |        |

## Reservation - Cancel Reservation

| ID        | Status | Remark |
|-----------|--------|--------|
| H02.FR.01 |        |        |
| H02.FR.02 |        |        |

## Reservation - Use a reserved EVSE

| ID        | Status | Remark |
|-----------|--------|--------|
| H03.FR.01 |        |        |
| H03.FR.02 |        |        |
| H03.FR.03 |        |        |
| H03.FR.04 |        |        |
| H03.FR.05 |        |        |
| H03.FR.06 |        |        |
| H03.FR.07 |        |        |
| H03.FR.08 |        |        |

## Reservation - Reservation Ended, not used

| ID        | Status | Remark |
|-----------|--------|--------|
| H04.FR.01 |        |        |
| H04.FR.02 |        |        |
| H04.FR.03 |        |        |

## TariffAndCost - Show EV Driver-specific Tariff Information

| ID        | Status | Remark |
|-----------|--------|--------|
| I01.FR.01 |        |        |
| I01.FR.02 |        |        |
| I01.FR.03 |        |        |

## TariffAndCost - Show EV Driver Running Total Cost During Charging

| ID        | Status | Remark |
|-----------|--------|--------|
| I02.FR.01 |        |        |
| I02.FR.02 |        |        |
| I02.FR.03 |        |        |
| I02.FR.04 |        |        |

## TariffAndCost -  Show EV Driver Final Total Cost After Charging

| ID        | Status | Remark |
|-----------|--------|--------|
| I03.FR.01 |        |        |
| I03.FR.02 |        |        |
| I03.FR.03 |        |        |
| I03.FR.04 |        |        |
| I03.FR.05 |        |        |

## TariffAndCost - Show Fallback Tariff Information

| ID        | Status | Remark |
|-----------|--------|--------|
| I04.FR.01 |        |        |
| I04.FR.02 |        |        |

## TariffAndCost - Show Fallback Total Cost Message

| ID        | Status | Remark |
|-----------|--------|--------|
| I05.FR.01 |        |        |
| I05.FR.02 |        |        |

## TariffAndCost - Update Tariff Information During Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| I06.FR.01 |        |        |
| I06.FR.02 |        |        |
| I06.FR.03 |        |        |

## MeterValues - Sending Meter Values not related to a transaction

| ID        | Status | Remark                                       |
|-----------|--------|----------------------------------------------|
| J01.FR.01 | ✅     |                                              |
| J01.FR.02 | ✅     |                                              |
| J01.FR.03 | ✅     |                                              |
| J01.FR.04 | ✅     |                                              |
| J01.FR.05 | ✅     |                                              |
| J01.FR.06 | ✅     |                                              |
| J01.FR.07 | ✅     |                                              |
| J01.FR.08 | ✅     |                                              |
| J01.FR.09 | ❎     | Location is provided by `libocpp` user.      |
| J01.FR.10 | ✅     |                                              |
| J01.FR.11 | ✅     |                                              |
| J01.FR.13 |        | Added phase rotation configuration variable. |
| J01.FR.14 | ✅     |                                              |
| J01.FR.15 | ❎     | tbd                                          |
| J01.FR.17 | ✅     |                                              |
| J01.FR.18 | ✅     |                                              |
| J01.FR.19 | ✅     |                                              |
| J01.FR.20 | ✅     |                                              |
| J01.FR.21 | ❎     | not valid                                    |

## MeterValues - Sending transaction related Meter Values

| ID        | Status | Remark                                       |
|-----------|--------|----------------------------------------------|
| J02.FR.01 | ✅     |                                              |
| J02.FR.02 | ✅     |                                              |
| J02.FR.03 | ✅     |                                              |
| J02.FR.04 | ✅     |                                              |
| J02.FR.05 | ✅     |                                              |
| J02.FR.06 | ✅     |                                              |
| J02.FR.07 | ✅     |                                              |
| J02.FR.09 |        | Added phase rotation configuration variable. |
| J02.FR.10 | ✅     |                                              |
| J02.FR.11 | ✅     |                                              |
| J02.FR.12 | ❎     | tbd                                          |
| J02.FR.13 | ❎     | tbd                                          |
| J02.FR.14 | ❎     | tbd                                          |
| J02.FR.16 | ❎     |                                              |
| J02.FR.17 | ❎     | tbd                                          |
| J02.FR.18 | ✅     |                                              |
| J02.FR.19 | ✅     |                                              |
| J02.FR.20 | ✅     |                                              |
| J02.FR.21 | ❎     | Signed meter values are not yet applicable.  |

## MeterValues - Charging Loop with metering information exchange

| ID        | Status | Remark |
|-----------|--------|--------|
| J03.FR.04 |        |        |


## SmartCharging - SetChargingProfile

| ID        | Status | Remark                                                                                          |
|-----------|--------|-------------------------------------------------------------------------------------------------|
| K01.FR.01 | 🌐     | `TxProfile`s are supported.                                                                     |
| K01.FR.02 | 🌐     |                                                                                                 |
| K01.FR.03 | 🌐 💂  | `TxProfile`s without `transactionId`s are rejected.                                             |
| K01.FR.04 | ✅     |                                                                                                 |
| K01.FR.05 | ✅     |                                                                                                 |
| K01.FR.06 | 🌐     |                                                                                                 |
| K01.FR.07 | ⛽️     | Notified through the `signal_set_charging_profiles` callback.                                   |
| K01.FR.08 | 🌐     | `TxDefaultProfile`s are supported.                                                              |
| K01.FR.09 | ✅     |                                                                                                 |
| K01.FR.10 | ✅     |                                                                                                 |
| K01.FR.11 |        |                                                                                                 |
| K01.FR.12 |        |                                                                                                 |
| K01.FR.13 |        |                                                                                                 |
| K01.FR.14 | ✅     |                                                                                                 |
| K01.FR.15 | ✅     |                                                                                                 |
| K01.FR.16 | ✅     |                                                                                                 |
| K01.FR.17 |        |                                                                                                 |
| K01.FR.19 |        |                                                                                                 |
| K01.FR.20 | ✅     | Suggests `ACPhaseSwitchingSupported` should be per EVSE, conflicting with the rest of the spec. |
| K01.FR.21 |        |                                                                                                 |
| K01.FR.22 |        |                                                                                                 |
| K01.FR.26 | ✅     |                                                                                                 |
| K01.FR.27 | ✅     |                                                                                                 |
| K01.FR.28 | ✅     |                                                                                                 |
| K01.FR.29 |        |                                                                                                 |
| K01.FR.30 |        |                                                                                                 |
| K01.FR.31 |        |                                                                                                 |
| K01.FR.32 | ✅     |                                                                                                 |
| K01.FR.33 | ✅     |                                                                                                 |
| K01.FR.34 | ✅     |                                                                                                 |
| K01.FR.35 | ✅     |                                                                                                 |
| K01.FR.36 | ✅     |                                                                                                 |
| K01.FR.37 |        |                                                                                                 |
| K01.FR.38 | 🌐 💂  | `ChargingStationMaxProfile`s with `Relative` for `chargingProfileKind` are rejected.            |
| K01.FR.39 | 🌐 💂  | New `TxProfile`s matching existing `(stackLevel, transactionId)` are rejected.                  |
| K01.FR.40 | 🌐 💂  | `Absolute`/`Recurring` profiles without `startSchedule` fields are rejected.                    |
| K01.FR.41 | 🌐 💂  | `Relative` profiles with `startSchedule` fields are rejected.                                   |
| K01.FR.42 |        |                                                                                                 |
| K01.FR.43 |        |                                                                                                 |
| K01.FR.44 | ✅     | We reject invalid profiles instead of modifying and accepting them.                             |
| K01.FR.45 | ✅     | We reject invalid profiles instead of modifying and accepting them.                             |
| K01.FR.46 |        |                                                                                                 |
| K01.FR.47 |        |                                                                                                 |
| K01.FR.48 |        |                                                                                                 |
| K01.FR.49 | ✅     |                                                                                                 |
| K01.FR.50 |        |                                                                                                 |
| K01.FR.51 |        |                                                                                                 |
| K01.FR.52 | ✅     |                                                                                                 |
| K01.FR.53 | ✅     |                                                                                                 |

## SmartCharging - Central Smart Charging

| ID        | Status | Remark                                           |
|-----------|--------|--------------------------------------------------|
| K02.FR.01 | ❎     |                                                  |
| K02.FR.02 | ❎     | This should be handled by the user of `libocpp`. |
| K02.FR.03 | ❎     |                                                  |
| K02.FR.04 |        |                                                  |
| K02.FR.05 |        |                                                  |
| K02.FR.06 |        | The same as K01.FR.21                            |
| K02.FR.07 |        | The same as K01.FR.22                            |
| K02.FR.08 |        |                                                  |

## SmartCharging - Local Smart Charging

| ID        | Status | Remark                |
|-----------|--------|-----------------------|
| K03.FR.01 | ❎     |                       |
| K03.FR.02 |        |                       |
| K03.FR.03 |        |                       |
| K03.FR.04 |        |                       |
| K03.FR.05 |        |                       |
| K03.FR.06 |        |                       |
| K03.FR.07 |        | The same as K01.FR.21 |
| K03.FR.08 |        | The same as K01.FR.22 |

## SmartCharging - Internal Load Balancing

| ID        | Status | Remark                                           |
|-----------|--------|--------------------------------------------------|
| K04.FR.01 |        |                                                  |
| K04.FR.02 |        |                                                  |
| K04.FR.03 | ✅     |                                                  |
| K04.FR.04 |        | The same as K01.FR.21                            |
| K04.FR.05 |        | This should be handled by the user of `libocpp`. |

## SmartCharging - Remote Start Transaction with Charging Profile

| ID        | Status | Remark |
|-----------|--------|--------|
| K05.FR.01 | ❎     |        |
| K05.FR.02 |        |        |
| K05.FR.03 |        |        |
| K05.FR.04 |        |        |
| K05.FR.05 |        |        |

## SmartCharging - Offline Behavior Smart Charging During Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| K06.FR.01 |        |        |
| K06.FR.02 |        |        |

## SmartCharging - Offline Behavior Smart Charging at Start of Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| K07.FR.01 |        |        |

## SmartCharging - Get Composite Schedule

| ID        | Status | Remark |
|-----------|--------|--------|
| K08.FR.01 | ❎     |        |
| K08.FR.02 |        |        |
| K08.FR.03 |        |        |
| K08.FR.04 |        |        |
| K08.FR.05 |        |        |
| K08.FR.06 |        |        |
| K08.FR.07 |        |        |

## SmartCharging - Get Charging Profiles

| ID        | Status | Remark |
|-----------|--------|--------|
| K09.FR.01 |        |        |
| K09.FR.02 |        |        |
| K09.FR.03 |        |        |
| K09.FR.04 |        |        |
| K09.FR.05 |        |        |
| K09.FR.06 |        |        |

## SmartCharging - Clear Charging Profile

| ID        | Status | Remark |
|-----------|--------|--------|
| K10.FR.01 |        |        |
| K10.FR.02 | ❎     |        |
| K10.FR.03 |        |        |
| K10.FR.04 |        |        |
| K10.FR.05 |        |        |
| K10.FR.06 |        |        |
| K10.FR.07 |        |        |
| K10.FR.08 |        |        |
| K10.FR.09 |        |        |

## SmartCharging - Set / Update External Charging Limit With Ongoing Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| K11.FR.01 |        |        |
| K11.FR.02 |        |        |
| K11.FR.03 |        |        |
| K11.FR.04 |        |        |
| K11.FR.05 |        |        |
| K11.FR.06 |        |        |

## SmartCharging - Set / Update External Charging Limit Without Ongoing Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| K12.FR.01 |        |        |
| K12.FR.02 |        |        |
| K12.FR.03 |        |        |
| K12.FR.04 |        |        |
| K12.FR.05 |        |        |

## SmartCharging - Reset / Release External Charging Limit

| ID        | Status | Remark |
|-----------|--------|--------|
| K13.FR.01 |        |        |
| K13.FR.02 |        |        |
| K13.FR.03 |        |        |

## SmartCharging - External Charging Limit with Local Controller

| ID        | Status | Remark |
|-----------|--------|--------|
| K14.FR.01 |        |        |
| K14.FR.02 |        |        |
| K14.FR.03 |        |        |
| K14.FR.04 |        |        |
| K14.FR.05 |        |        |
| K14.FR.06 |        |        |

## SmartCharging - Charging with load leveling based on High Level Communication

| ID        | Status | Remark |
|-----------|--------|--------|
| K15.FR.01 |        |        |
| K15.FR.02 |        |        |
| K15.FR.03 |        |        |
| K15.FR.04 |        |        |
| K15.FR.05 |        |        |
| K15.FR.06 |        |        |
| K15.FR.07 |        |        |
| K15.FR.08 |        |        |
| K15.FR.09 |        |        |
| K15.FR.10 |        |        |
| K15.FR.11 |        |        |
| K15.FR.12 |        |        |
| K15.FR.13 |        |        |
| K15.FR.14 |        |        |
| K15.FR.15 |        |        |
| K15.FR.16 |        |        |
| K15.FR.17 |        |        |
| K15.FR.18 |        |        |
| K15.FR.19 |        |        |

## SmartCharging - Renegotiation initiated by CSMS

| ID        | Status | Remark |
|-----------|--------|--------|
| K16.FR.01 |        |        |
| K16.FR.02 |        |        |
| K16.FR.03 |        |        |
| K16.FR.04 |        |        |
| K16.FR.05 |        |        |
| K16.FR.06 |        |        |
| K16.FR.07 |        |        |
| K16.FR.08 |        |        |
| K16.FR.09 |        |        |
| K16.FR.10 |        |        |
| K16.FR.11 |        |        |
| K16.FR.12 |        |        |
| K16.FR.13 |        |        |

## SmartCharging - Renegotiation initiated by EV

| ID        | Status | Remark |
|-----------|--------|--------|
| K17.FR.01 |        |        |
| K17.FR.02 |        |        |
| K17.FR.03 |        |        |
| K17.FR.04 |        |        |
| K17.FR.05 |        |        |
| K17.FR.06 |        |        |
| K17.FR.07 |        |        |
| K17.FR.08 |        |        |
| K17.FR.09 |        |        |
| K17.FR.10 |        |        |
| K17.FR.11 |        |        |
| K17.FR.12 |        |        |
| K17.FR.13 |        |        |
| K17.FR.14 |        |        |
| K17.FR.15 |        |        |
| K17.FR.16 |        |        |

## FirmwareManagement - Secure Firmware Update

| ID        | Status | Remark                                      |
|-----------|--------|---------------------------------------------|
| L01.FR.01 | ⛽️     |                                             |
| L01.FR.02 | ✅     | Security Notification is sent by `libocpp`. |
| L01.FR.03 | ✅     | Security Notification is sent by `libocpp`. |
| L01.FR.04 | ⛽️     |                                             |
| L01.FR.05 | ⛽️     |                                             |
| L01.FR.06 | ⛽️     |                                             |
| L01.FR.07 | ⛽️     |                                             |
| L01.FR.08 | ❎     | Recommendation, not a requirement           |
| L01.FR.09 | 🤓     | Requirement on the firmware file itself.    |
| L01.FR.10 | ⛽️     |                                             |
| L01.FR.11 | 🌐     |                                             |
| L01.FR.12 | ⛽️     |                                             |
| L01.FR.13 | ⛽️     |                                             |
| L01.FR.14 | ⛽️     |                                             |
| L01.FR.15 | ⛽️     |                                             |
| L01.FR.16 | ⛽️     |                                             |
| L01.FR.20 | ✅     |                                             |
| L01.FR.21 | ⛽️     |                                             |
| L01.FR.22 | ⛽️     |                                             |
| L01.FR.23 | ⛽️     |                                             |
| L01.FR.24 | ⛽️     |                                             |
| L01.FR.25 | ✅     |                                             |
| L01.FR.26 | ✅     |                                             |
| L01.FR.27 |        | Optional requirement                        |
| L01.FR.28 | ⛽️     |                                             |
| L01.FR.29 | ⛽️     |                                             |
| L01.FR.30 | ⛽️     |                                             |
| L01.FR.31 | ✅     |                                             |
| L01.FR.32 | ❎     | Optional requirement                        |

## FirmwareManagement - Non-Secure Firmware Update

| ID        | Status | Remark |
|-----------|--------|--------|
| L02.FR.01 | ⛽️     |        |
| L02.FR.02 | ⛽️     |        |
| L02.FR.03 | ⛽️     |        |
| L02.FR.04 | ⛽️     |        |
| L02.FR.05 | ⛽️     |        |
| L02.FR.06 | ⛽️     |        |
| L02.FR.07 | ⛽️     |        |
| L02.FR.08 | ⛽️     |        |
| L02.FR.09 | ⛽️     |        |
| L02.FR.10 | ⛽️     |        |
| L02.FR.14 | ⛽️     |        |
| L02.FR.15 | ⛽️     |        |
| L02.FR.16 | ✅     |        |
| L02.FR.17 | ✅     |        |
| L02.FR.18 | ⛽️     |        |
| L02.FR.19 | ⛽️     |        |
| L02.FR.20 | ⛽️     |        |
| L02.FR.21 | ⛽️     |        |

## FirmwareManagement - Publish Firmware file on Local Controller

| ID        | Status | Remark |
|-----------|--------|--------|
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
|-----------|--------|--------|
| L04.FR.01 |        |        |
| L04.FR.02 |        |        |
| L04.FR.03 |        |        |
| L04.FR.04 |        |        |

## ISO 15118 CertificateManagement - Certificate installation EV

| ID        | Status | Remark |
|-----------|--------|--------|
| M01.FR.01 | ✅     |        |

## ISO 15118 CertificateManagement - Certificate Update EV

| ID        | Status | Remark |
|-----------|--------|--------|
| M02.FR.01 | ✅     |        |

## ISO 15118 CertificateManagement - Retrieve list of available certificates from a Charging Station

| ID        | Status | Remark |
|-----------|--------|--------|
| M03.FR.01 | ✅     |        |
| M03.FR.02 | ✅     |        |
| M03.FR.03 | ✅     |        |
| M03.FR.04 | ✅     |        |
| M03.FR.05 | ✅     |        |

## ISO 15118 CertificateManagement - Delete a specific certificate from a Charging Station

| ID        | Status | Remark                               |
|-----------|--------|--------------------------------------|
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
|-----------|--------|--------|
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
|-----------|--------|--------|
| M06.FR.01 | ❎     |        |
| M06.FR.02 | ❎     |        |
| M06.FR.03 | ❎     |        |
| M06.FR.04 | ❎     |        |
| M06.FR.06 | ✅     |        |
| M06.FR.07 |        |        |
| M06.FR.08 | ❎     |        |
| M06.FR.09 | ❎     |        |
| M06.FR.10 | ✅     |        |

## Diagnostics - Retrieve Log Information

| ID        | Status | Remark |
|-----------|--------|--------|
| N01.FR.01 | ✅     |        |
| N01.FR.02 | ⛽️     |        |
| N01.FR.03 | ⛽️     |        |
| N01.FR.04 | ⛽️     |        |
| N01.FR.05 | ⛽️     |        |
| N01.FR.06 | ⛽️     |        |
| N01.FR.07 | ⛽️     |        |
| N01.FR.08 | ⛽️     |        |
| N01.FR.09 | ⛽️     |        |
| N01.FR.10 | ⛽️     |        |
| N01.FR.11 | ⛽️     |        |
| N01.FR.12 | ⛽️     |        |
| N01.FR.13 | ⛽️     |        |
| N01.FR.14 | ⛽️     |        |
| N01.FR.15 | ⛽️     |        |
| N01.FR.16 | ⛽️     |        |
| N01.FR.17 | ⛽️     |        |
| N01.FR.18 | ⛽️     |        |
| N01.FR.19 | ⛽️     |        |
| N01.FR.20 | ⛽️     |        |

## Diagnostics - Get Monitoring report

| ID        | Status | Remark                                   |
|-----------|--------|------------------------------------------|
| N02.FR.01 | ✅     |                                          |
| N02.FR.02 | ❎     | Everything is supported by our charger   |
| N02.FR.03 | ✅     |                                          |
| N02.FR.04 | ✅     |                                          |
| N02.FR.05 | ✅     |                                          |
| N02.FR.06 | ✅     |                                          |
| N02.FR.07 | ✅     |                                          |
| N02.FR.08 | ✅     |                                          |
| N02.FR.09 | ✅     |                                          |
| N02.FR.10 | ✅     |                                          |
| N02.FR.11 | ✅     |                                          |
| N02.FR.12 | ✅     |                                          |
| N02.FR.13 | ✅     |                                          |
| N02.FR.14 | ✅     |                                          |
| N02.FR.15 | ✅     |                                          |
| N02.FR.16 | ✅     |                                          |
| N02.FR.17 | ✅     |                                          |
| N02.FR.18 |        |                                          |
| N02.FR.19 |        |                                          |
| N02.FR.20 |        |                                          |
| N02.FR.21 |        |                                          |

## Diagnostics - Set Monitoring Base

| ID        | Status | Remark                                   |
|-----------|--------|------------------------------------------|
| N03.FR.01 | ✅     |                                          |
| N03.FR.02 | ❎     | Everything is supported on our charger   |
| N03.FR.03 | ✅     |                                          |
| N03.FR.04 | ✅     |                                          |
| N03.FR.05 | ✅     |                                          |

## Diagnostics - Set Variable Monitoring

| ID        | Status | Remark |
|-----------|--------|--------|
| N04.FR.01 | ✅     |        |
| N04.FR.02 | ✅     |        |
| N04.FR.03 | ✅     |        |
| N04.FR.04 | ✅     |        |
| N04.FR.05 |        |        |
| N04.FR.06 | ✅     |        |
| N04.FR.07 | ✅     |        |
| N04.FR.08 | ✅     |        |
| N04.FR.09 | ✅     |        |
| N04.FR.10 | ✅     |        |
| N04.FR.11 | ✅     |        |
| N04.FR.12 | ✅     |        |
| N04.FR.13 | ✅     |        |
| N04.FR.14 | ✅     |        |
| N04.FR.15 |        |        |
| N04.FR.16 | ✅     |        |
| N04.FR.17 |        |        |
| N04.FR.18 | ✅     |        |
| N04.FR.19 | ✅     |        |

## Diagnostics - Set Monitoring Level

| ID        | Status | Remark |
|-----------|--------|--------|
| N05.FR.01 | ✅     |        |
| N05.FR.02 | ✅     |        |
| N05.FR.03 | ✅     |        |

## Diagnostics - Clear / Remove Monitoring

| ID        | Status | Remark |
|-----------|--------|--------|
| N06.FR.01 | ✅     |        |
| N06.FR.02 | ✅     |        |
| N06.FR.03 | ✅     |        |
| N06.FR.04 | ✅     |        |
| N06.FR.05 | ✅     |        |
| N06.FR.06 | ✅     |        |
| N06.FR.07 | ✅     |        |

## Diagnostics - Alert Event

| ID        | Status | Remark |
|-----------|--------|--------|
| N07.FR.02 | ✅     |        |
| N07.FR.03 | ✅     |        |
| N07.FR.04 | ✅     |        |
| N07.FR.05 |        |        |
| N07.FR.06 | ✅     |        |
| N07.FR.07 | ✅     |        |
| N07.FR.10 | ✅     |        |
| N07.FR.11 |        |        |
| N07.FR.12 |        |        |
| N07.FR.13 | ✅     |        |
| N07.FR.14 |        |        |
| N07.FR.15 | ✅     |        |
| N07.FR.16 | ✅     |        |
| N07.FR.17 | ✅     |        |
| N07.FR.18 | ✅     |        |
| N07.FR.19 | ✅     |        |

## Diagnostics - Periodic Event

| ID        | Status | Remark |
|-----------|--------|--------|
| N08.FR.02 | ✅     |        |
| N08.FR.03 |        |        |
| N08.FR.04 | ✅     |        |
| N08.FR.05 | ✅     |        |
| N08.FR.06 | ✅     |        |
| N08.FR.07 | ✅     |        |

## Diagnostics - Get Customer Information

| ID        | Status | Remark |
|-----------|--------|--------|
| N09.FR.01 | ❎     |        |
| N09.FR.02 | ✅     |        |
| N09.FR.03 | ✅     |        |
| N09.FR.04 | ❎     |        |
| N09.FR.05 | ✅     |        |
| N09.FR.06 | ✅     |        |
| N09.FR.07 | ✅     |        |
| N09.FR.08 | ❎     |        |

## Diagnostics - Clear Customer Information

| ID        | Status | Remark |
|-----------|--------|--------|
| N10.FR.01 | ✅     |        |
| N10.FR.02 | ❎     |        |
| N10.FR.03 | ✅     |        |
| N10.FR.04 | ✅     |        |
| N10.FR.05 | ✅     |        |
| N10.FR.06 | ✅     |        |
| N10.FR.07 | ✅     |        |
| N10.FR.08 | ❎     |        |
| N10.FR.09 | ❎     |        |

## DisplayMessage - Set DisplayMessage

| ID        | Status | Remark |
|-----------|--------|--------|
| O01.FR.01 |        |        |
| O01.FR.02 |        |        |
| O01.FR.03 |        |        |
| O01.FR.04 |        |        |
| O01.FR.05 |        |        |
| O01.FR.06 |        |        |
| O01.FR.07 |        |        |
| O01.FR.08 |        |        |
| O01.FR.09 |        |        |
| O01.FR.10 |        |        |
| O01.FR.11 |        |        |
| O01.FR.12 |        |        |
| O01.FR.13 |        |        |
| O01.FR.14 |        |        |
| O01.FR.15 |        |        |
| O01.FR.16 |        |        |
| O01.FR.17 |        |        |

## DisplayMessage - Set DisplayMessage for Transaction

| ID        | Status | Remark |
|-----------|--------|--------|
| O02.FR.01 |        |        |
| O02.FR.02 |        |        |
| O02.FR.03 |        |        |
| O02.FR.04 |        |        |
| O02.FR.05 |        |        |
| O02.FR.06 |        |        |
| O02.FR.07 |        |        |
| O02.FR.08 |        |        |
| O02.FR.09 |        |        |
| O02.FR.10 |        |        |
| O02.FR.11 |        |        |
| O02.FR.12 |        |        |
| O02.FR.14 |        |        |
| O02.FR.15 |        |        |
| O02.FR.16 |        |        |
| O02.FR.17 |        |        |
| O02.FR.18 |        |        |

## DisplayMessage - Get All DisplayMessages

| ID        | Status | Remark |
|-----------|--------|--------|
| O03.FR.02 |        |        |
| O03.FR.03 |        |        |
| O03.FR.04 |        |        |
| O03.FR.05 |        |        |
| O03.FR.06 |        |        |

## DisplayMessage - Get Specific DisplayMessages

| ID        | Status | Remark |
|-----------|--------|--------|
| O04.FR.01 |        |        |
| O04.FR.02 |        |        |
| O04.FR.03 |        |        |
| O04.FR.04 |        |        |
| O04.FR.05 |        |        |
| O04.FR.06 |        |        |
| O04.FR.07 |        |        |

## DisplayMessage - Clear a DisplayMessage

| ID        | Status | Remark |
|-----------|--------|--------|
| O05.FR.01 |        |        |
| O05.FR.02 |        |        |

## DisplayMessage - Replace DisplayMessage

| ID        | Status | Remark |
|-----------|--------|--------|
| O06.FR.01 |        |        |

## DataTransfer - Data Transfer to the Charging Station

| ID        | Status | Remark                                                    |
|-----------|--------|-----------------------------------------------------------|
| P01.FR.01 | ✅     | There is no way yet to register a data transfer callback. |
| P01.FR.02 | ❎     |                                                           |
| P01.FR.03 | ❎     |                                                           |
| P01.FR.04 | ❎     |                                                           |
| P01.FR.05 | ✅     |                                                           |
| P01.FR.06 | ✅     |                                                           |
| P01.FR.07 | ❎     |                                                           |

## DataTransfer - Data Transfer to the CSMS

| ID        | Status | Remark |
|-----------|--------|--------|
| P02.FR.01 | ⛽️     |        |
| P02.FR.02 | ⛽️     |        |
| P02.FR.03 | ❎     |        |
| P02.FR.04 | ⛽️     |        |
| P02.FR.05 | ❎     |        |
| P02.FR.06 | ❎     |        |
| P02.FR.07 | ❎     |        |
| P02.FR.08 | ❎     |        |
