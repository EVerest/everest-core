# Notes on Smart Charging Profiles and approach
There are some complexities calculating composite schedules and this note explains the approach.

The new approach is in `profile.cpp` and `profile.hpp` and then integrated into `smart_charging.cpp` maintaining the same API.

```
    std::vector<ChargingProfile> get_valid_profiles(
      const DateTime& start_time, const DateTime& end_time,
      int connector_id);

    EnhancedChargingSchedule calculate_enhanced_composite_schedule(
      const std::vector<ChargingProfile>& valid_profiles,
      const DateTime& start_time,
      const DateTime& end_time,
      const int connector_id,
      std::optional<ChargingRateUnit> charging_rate_unit);
```

## get_valid_profiles()
Retrieves all profiles that should be considered for calculating the composite schedule.
- checks that profiles are associated with the correct connector
- checks that profiles match the transaction ID (when there is an active transaction)
- start_time and end_time are not used in the new implementation

`start_time` and `end_time` could be used to remove profiles that would never be valid
within that period. However only the `validFrom` and `validTo` settings should be considered.

`calculate_enhanced_composite_schedule` checks the `start_time` and `end_time` so it is not essential to remove profiles at this point.

## calculate_enhanced_composite_schedule()
Assumes that profiles for other connectors and transactions have been removed.

Processes profiles:
1. obtain session/transaction start time
2. split the profiles into ChargePointMax/TxDefault/Tx sets
3. for each set calculate the composite schedule using the preferred units (Amps/Watts)
   the resulting composite schedule covers the time period `start_time` to `end_time` only. Note that stack level is used where the higher level has priority
4. combines the three composite schedules into the single result

When combining; TxDefault is the starting limit, overridden by Tx and potentially lowered by ChargePointMax. The result will never be higher than a ChargePointMax limit (where there is one).

## Time handling
The approach removes all calls for obtaining the current date and time.

- where there is a transaction then the start time of the transaction is obtained
- relative schedules use the transaction start time where known or the start time

For generating a composite schedule relative schedules are included based on the transaction start time. Where there isn't a transaction in progress the `start_time` is used - i.e. the assumption is that a transaction has just started.

The removal of any relationship to the current time simplifies writing test cases and debugging test failures.

## Default limit
The OCPP 1.6 specification doesn't support gaps in charging schedules. This presents a problem while creating a composite schedule when there is a period of time when no profile is active.

- profile 1: stack level 10, Monday 10:00 for 2 hours
- profile 2: stack level 11, Monday 14:00 for 2 hours

At Monday 08:00 requesting a composite schedule for the next 9 hours needs to indicate

08:00|09:00|10:00|11:00|12:00|13:00|14:00|15:00|16:00|17:00
-----|-----|-----|-----|-----|-----|-----|-----|-----|-----
unknown|unknown|P1|P1|unknown|unknown|P2|P2|unknown|unknown|unknown

Where the limit is not known then a default limit of `48.0 Amps` is used when calculating the final composite schedule.

A different default can be specified by installing a lower stack level TxDefault profile e.g.

```
{
  "chargingProfileId": 1,
  "chargingProfileKind": "Relative",
  "chargingProfilePurpose": "TxDefaultProfile",
  "stackLevel": 1
  "chargingSchedule": {
    "chargingRateUnit": "A",
    "chargingSchedulePeriod": [
      {
        "limit": 32.0,
        "startPeriod": 0
      }
    ],
  },
}
```

## No valid profiles
Since a default limit is applied a composite schedule will always start at the `start_time` and have a fixed duration even when there are no valid profiles for that time period.

e.g. for 2024-01-01 starting at 08:00 for 10 minutes

```
{
  "status": "Accepted",
  "scheduleStart": "2024-01-01T08:00:00Z",
  "chargingSchedule": {
    "duration:": 3600,
    "startSchedule": "2024-01-01T08:00:00Z",
    "chargingRateUnit": "A",
    "chargingSchedulePeriod": {
      "startPeriod": 0,
      "limit": 0.0
    }
  }
}
```

When building the OCPP response "startSchedule" could be excluded however the composite schedule refers to a specific point in time, so it should be provided.

According to the OCPP specification section `7.13 ChargingSchedule` chargingSchedulePeriod is a required field and must have at least one entry. Hence the following is not a valid response:

```
{
  "status": "Accepted",
  "scheduleStart": "2024-01-01T08:00:00Z",
  "chargingSchedule": {
    "duration:": 3600,
    "startSchedule": "2024-01-01T08:00:00Z",
    "chargingRateUnit": "A",
    "chargingSchedulePeriod": {}
  }
}
```

## Profile validity

The following items need to be considered when looking at a schedule:
- validFrom
- validTo
- transaction start time
- startSchedule
- duration
- startPeriod

The following sections explore some interesting edge cases.


### validFrom & validTo and transaction start time

For the following schedule
```
{
  "chargingProfileId": 1,
  "chargingProfileKind": "Relative",
  "chargingProfilePurpose": "TxDefaultProfile",
  "stackLevel": 1,
  "chargingSchedule": {
    "chargingRateUnit": "A",
    "chargingSchedulePeriod": [
      {
        "limit": 32.0,
        "startPeriod": 0
      },
      {
        "limit": 6.0,
        "startPeriod": 3600
      }
    ],
  },
  "validFrom": "2024-01-01T12:00:00Z"
  "validTo": "2024-01-01T20:00:00Z"
}
```

The PEV plugs in at "2024-01-01T10:00:00Z" the result is no charging until the profile is valid at 12:00 and then the limit is 6.0A since only the first hour of a transaction is at the higher limit of 32.0A

The PEV plugs in at "2024-01-01T19:50:00Z" the result is charging for 10 minutes at 32. A and then no charge offered.

### startSchedule and daily recurring

For the following schedule
```
{
  "chargingProfileId": 1,
  "chargingProfileKind": "Recurring",
  "chargingProfilePurpose": "TxDefaultProfile",
  "recurrencyKind": "Daily",
  "stackLevel": 1,
  "chargingSchedule": {
    "startSchedule": "2024-01-01T12:00:00Z",
    "chargingRateUnit": "A",
    "chargingSchedulePeriod": [
      {
        "limit": 32.0,
        "startPeriod": 0
      },
      {
        "limit": 6.0,
        "startPeriod": 3600
      }
    ],
  },
}
```

The PEV plugs in at "2024-01-10T11:50:00Z" the result is charging at 6.0A for 10 minutes (based on the previous day) and then at 32.0A from 12:00 when the schedule repeats.

`validFrom` and `validTo` add additional complications.

### startSchedule and daily recurring with duration

For the following schedule
```
{
  "chargingProfileId": 1,
  "chargingProfileKind": "Recurring",
  "chargingProfilePurpose": "TxDefaultProfile",
  "recurrencyKind": "Daily",
  "stackLevel": 1,
  "chargingSchedule": {
    "startSchedule": "2024-01-01T12:00:00Z",
    "duration": 18000,
    "chargingRateUnit": "A",
    "chargingSchedulePeriod": [
      {
        "limit": 32.0,
        "startPeriod": 0
      },
      {
        "limit": 6.0,
        "startPeriod": 3600
      }
    ],
  },
  "validFrom": "2024-02-01T12:00:00Z"
  "validTo": "2024-03-01T09:00:00Z"
}
```

The PEV plugs in at "2024-02-10T11:50:00Z" the result is based on the default limit since the profile is only valid for 5 hours. i.e. no charging for 10 minutes (based on the previous day) and then at 32.0A from 12:00 when the schedule repeats.

The PEV plugs in at "2024-02-01T11:50:00Z" the result is based on the default limit since the profile isn't valid yet. i.e. no charging for 10 minutes and then at 32.0A from 12:00 when the schedule starts.

The PEV plugs in at "2024-03-01T08:00:00Z" the result is charging at 6.0A (based on the previous day) and then no charging at 09:00 when the profile is no longer valid.