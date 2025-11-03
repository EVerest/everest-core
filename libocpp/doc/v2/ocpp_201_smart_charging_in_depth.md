# Smart Charging Use Cases

The use cases within the Smart Charging functional block are subdivided into the following three categories of use cases:

1. General Smart Charging (Use Cases K01–K10)
2. External Charging Limit-based Smart Charging (K11–K14)
3. ISO 15118-based Smart Charging (K15–K17)

Support for General and External Charging Limit-based Smart Charging is largely complete, with ISO 15118-based Smart Charging under active development. For an up-to-date overview of exactly which features are currently supported as well as design decisions that have been made to address optional or ambiguous functional requirements, please refer to the [OCPP 2.0.1 Status document](ocpp_2x_status.md).

## K01 SetChargingProfile

Allows the CSMS to influence the charging power or current drawn from a specific EVSE or the
entire Charging Station over a period of time.

```mermaid
sequenceDiagram
    CSMS->>+ChargePoint : SetChargingProfileRequest(call)

    ChargePoint->>+DeviceModel : SmartChargingCtrlrAvailable?
    DeviceModel-->>-ChargePoint : Component

    rect rgb(128,202,255) 
    break SmartChargingCtrlrAvailable = false
        ChargePoint-->>CSMS : Smart Charging NotSupported CallError
    end
    end

    ChargePoint->>+SmartCharging : validate_and_add_profile(call.msg.Profile, call.msg.EVSE ID)

    SmartCharging->>SmartCharging : validate_profile(Profile, EVSE ID)

    rect rgb(128,202,255) 
    break Invalid Profile
        SmartCharging-->>ChargePoint : SetChargingProfileResponse: Rejected
        ChargePoint-->>CSMS : SetChargingProfileResponse: Rejected
    end
    end

    SmartCharging->>+SmartCharging : add_profile(Profile, EVSE ID)
    SmartCharging->>-EVerest : signal_set_charging_profiles_callback

    SmartCharging-->>-ChargePoint : SetChargingProfileResponse: Accepted

    ChargePoint-->>-CSMS : SetChargingProfileResponse: Accepted
```

Profile validation returns the following errors to the caller when a Profile
is `Rejected`:

| Errors                                                        | Description                                                                                                                                                                                                                                         |
| :------------------------------------------------------------ | :-------------------------------------------------------------- |
| `ChargingProfileFirstStartScheduleIsNotZero`                  | The `startPeriod` of the first `chargingSchedulePeriod` needs to be 0.<br>[K01.FR.31] |
| `ChargingProfileNoChargingSchedulePeriods`                    | Happens when the `ChargingProfile` doesn't have any Charging Schedule Periods. |
| `ChargingScheduleChargingRateUnitUnsupported`                 | Happens when a chargingRateUnit is passed in that is not configured in the `ChargingScheduleChargingRateUnit`. [K01.FR.26] |
| `ChargingSchedulePeriodInvalidPhaseToUse`                     | Happens when an invalid `phaseToUse` is passed in. [K01.FR.19] [K01.FR.48] |
| `ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported` | Happens when `phaseToUse` is passed in and the EVSE does not have `ACPhaseSwitchingSupported` defined and set to true. [K01.FR.20] [K01.FR.48] |
| `ChargingSchedulePeriodsOutOfOrder`                           | `ChargingSchedulePeriod.startPeriod` elements need to be in increasing values. [K01.FR.35] |
| `ChargingStationMaxProfileCannotBeRelative`                   | Happens when a `ChargingStationMaxProfile.chargingProfileKind` is set to `Relative`. [K01.FR.38] |
| `ChargingStationMaxProfileEvseIdGreaterThanZero`              | Happens when a `ChargingStationMaxProfile` is attempted to be set with an EvseID isn't `0`. [K01.FR.03] |
| `ChargingProfileMissingRequiredStartSchedule`                 | Happens when an `Absolute` or `Recurring` `ChargingProfile` doesn't have a `startSchedule`. [K01.FR.40] |
| `ChargingProfileExtraneousStartSchedule`                      | Happens when a Relative `ChargingProfile` has a `startSchedule`. [K01.FR.41] |
| `EvseDoesNotExist`                                            | Happens when the `evseId`of a `SetChargingProfileRequest` does not exist. [K01.FR.28] |
| `ExistingChargingStationExternalConstraints`                  | Happens when a `SetChargingProfileRequest` Profile has a purpose of `ChargingStationExternalConstraints` and one already exists with the same `ChargingProfile.id` exists. [K01.FR.05] |
| `InvalidProfileType`                                          | Happens when a `ChargingStationMaxProfile` is attempted to be set with a `ChargingProfile` that isn't a `ChargingStationMaxProfile`. |
| `TxProfileEvseHasNoActiveTransaction`                         | Happens when a `SetChargingProfileRequest` with a `TxProfile` is submitted and there is no transaction active on the specified EVSE. [K01.FR.09] |
| `TxProfileEvseIdNotGreaterThanZero`                           | `TxProfile` needs to have an `evseId` greater than 0. [K01.FR.16] |
| `TxProfileMissingTransactionId`                               | A `transactionId` is required for`SetChargingProfileRequest`s with a `TxProfile` in order to match the profile to a specific transation. [K01.FR.03] |
| `TxProfileTransactionNotOnEvse`                               | Happens when the provided `transactionId` is not known. [K01.FR.33] |
| `TxProfileConflictingStackLevel`                              | Happens when a `TxProfile` has a `stackLevel` and `transactionId` combination already exists in a `TxProfile` with a different id in order to ensure that no two charging profiles with same stack level and purpose can be valid at the same time. [K01.FR.39] |

## K08 Get Composite Schedule

The CSMS requests the Charging Station to report the Composite Charging
Schedule, as calculated by the Charging Station for a specific point of
time, and may change over time due to external causes such as local
balancing based on grid connection capacity and EVSE availablity.

The Composite Schedule is the result of result of merging the time periods
set in the `ChargingStationMaxProfile`, `ChargingStationExternalConstraints`,
`TxDefaultProfile` and `TxProfile` type profiles.

```mermaid
sequenceDiagram

    CSMS->>+ChargePoint: GetCompositeSchedule(call)

    ChargePoint->>+DeviceModel : ChargingScheduleChargingRateUnit?
    DeviceModel-->>-ChargePoint : Component

    rect rgb(128,202,255) 
        break call.msg.chargingRateUnit is not supported
            ChargePoint-->>CSMS : ChargingScheduleChargingRateUnitUnsupported CallError
        end
    end

    ChargePoint->>+EvseManager : does_evse_exist(call.msg.evseId)
    EvseManager-->>-ChargePoint : bool
    rect rgb(128,202,255) 
        break EVSE does not exist
            ChargePoint-->>CSMS : EvseDoesNotExist CallError
        end
    end

    ChargePoint->>+SmartChargingHandler : get_valid_profiles(call.msg.evseId)

    SmartChargingHandler-->>-ChargePoint : vector<ChargingProfile>

    ChargePoint->>+SmartChargingHandler : calculate_composite_schedule<br/>(vector<ChargingProfile, now, msg.duration, evseId, call.msg.chargingRateUnit)

    loop ExternalConstraints, Max, TxDefault, and Tx Profiles
        SmartChargingHandler->>+Profile: calculate_composite_schedule(profiles)
        Profile-->>-SmartChargingHandler: composite_schedule 
    end

    note right of SmartChargingHandler: Create consolidated CompositeSchedule<br />from all 4 Profile types


    SmartChargingHandler->>+Profile: calculate_composite_schedule(ExternalConstraints, Max, TxDefault, Tx)
    Profile-->>-SmartChargingHandler: CompositeSchedule

    SmartChargingHandler-->>-ChargePoint: CompositeSchedule

    ChargePoint-->>-CSMS : GetCompositeScheduleResponse(CompositeSchedule)
```

## K09 Get Charging Profiles

Returns to the CSMS the Charging Schedules/limits installed on a Charging Station based on the 
passed in criteria.

```mermaid
sequenceDiagram

    CSMS->>+ChargePoint: GetChargingProfiles(criteria)

    ChargePoint->>+SmartChargingHandler : get_reported_profiles(criteria)

    loop filter ChargingProfiles
        SmartChargingHandler->>SmartChargingHandler: filter on ChargingProfile criteria 
    end

    SmartChargingHandler-->>-ChargePoint : Vector<Profiles>

    ChargePoint-->>CSMS : GetChargingProfilesResponse(profiles)

    alt no Profiles
    rect rgb(128,202,255)
        ChargePoint-->>CSMS : GetChargingProfilesResponse(NoProfiles) 
        ChargePoint->>CSMS : return
    end
        
    else Profiles
        ChargePoint-->>CSMS : GetChargingProfilesResponse(Accepted)
    end

    ChargePoint->>ChargePoint : determine profiles_to_report

    ChargePoint-->>-CSMS : ReportChargingProfilesRequest(profiles_to_report)
```

## K10 Clear Charging Profile

Clears Charging Profiles installed on a Charging Station based on the
passed in criteria.

```mermaid
sequenceDiagram

    CSMS->>+ChargePoint: ClearChargingProfileRequest(criteria)

    alt no Profiles matching criteria
    rect rgb(128,202,255)
        ChargePoint-->>CSMS : ClearChargingProfileResponse(Unknown) 

    end
        
    else found matching Profiles
        ChargePoint-->>-CSMS : ClearChargingProfileResponse(Accepted)
    end    
```
