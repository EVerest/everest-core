# Baseline Profiles

Profiles created to use as a baseline for testing Composite Schedule scenarios.
The goal was to use actual Profiles from the standard as a baseline for Unit
and Integration Testing via tools such as
[Appenzell](https://github.com/US-JOET/appenzell).

## Profiles

`TxProfile_100.json` is based on the example profile on page 240 of the
`OCPP-2.0.1_part2_specification_edition2` document. The only substantial change
has been to change the 3rd `ChargingSchedulePeriod` limit to 12,000 so that it
can be more easily differentiated from the first period with a limit of 11,000.
Since it has a Stack Level of 0, any profile with a higher one will take
precedence.

`TxProfile_1.json` is based on the example profile on page 241 of the
`OCPP-2.0.1_part2_specification_edition2` document. With a Stack Level of 1,
any overlapping `ChargingSchedulePeriods` will take precedence.
