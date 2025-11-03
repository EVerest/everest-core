# Profiles for tests only requiring a single one

The following Profiles are all serialized JSON versions of ones instantiated
directly in the original `tests/lib/ocpp/v16/profile_testsA.cpp` OCPP 1.6
versions of the tests.

* singles/Absolute_301.json
* singles/Absolute_NoDuration_301.json
* singles/Relative_301.json
* singles/Relative_NoDuration_301.json
* singles/Recurring_Daily_301.json
* singles/Recurring_Daily_NoDuration_301.json
* singles/Recurring_Weekly_301.json
* singles/Recurring_Weekly_NoDuration_301.json

The goal is to clearly isolate out each profile, and simplify as much as
possible the writing of the tests by leveraging
[GoogleTests's parameter based testing feature](https://google.github.io/googletest/reference/testing.html#TEST_P),
greatly reducing the amount of boiler plate needed for the tests.

* singles/TXProfile_Absolute_Start18-04.json

This profile is used for a specific test scenario where any actual Profile
ChargingSchedulePeriod happens after the time window of the request.

* singles/TxProfile_CONCERNING_overlapping_periods.json

This is a Profile created with a vector of `ChargingSchedulePeriods` that is
longer in duraction for a single day, but is recurring daily so that they
will start to overlap after 24 hours. The idea is to create a Profile to test
this sort of edge case. Right now there aren't any tests using it.
