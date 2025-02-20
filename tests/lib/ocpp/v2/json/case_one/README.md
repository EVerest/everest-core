# Case One

Case one is a variation of the baseline scenario but with `TxProfile_1.json`
being `Absolute` instead of `Recurring`. It was designed to track down a
potential defect that was being tracked in the earlier version of the code.
See [Issue #609](https://github.com/EVerest/libocpp/issues/609).

The following tests are designed to isolate the issue:

* K08_CalculateCompositeSchedule_DemoCaseOne_17th
* K08_CalculateCompositeSchedule_DemoCaseOne_19th

In the first the time window matches both profiles, while in the second it
only matches the `Recurring` profile.
