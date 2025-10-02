# Dold RN5893 Isolation Monitor

## Self test

We currently only support "normal" self tests, not the extended self test.

When a self test is started via the IMD interface, bit 4 of the "control word 1" register (address 40001) is set.
The device will then perform a self test. If a device fault is present while the self test is running, the self test will fail.
If the device reports a state that is not "self test running" anymore and there is no device fault, the self test is considered successful.

Note that after setting the self test bit, it can take a few seconds (0-2s) until the device state reflects that a self test is running. This is solved using the internal `self_test_running` and `self_test_triggered` variables of the driver.

## Timeout

The device supports a communication timeout, which raises a device fault if no communication is possible for a certain time.

This module supports this timeout, which can be enabled by setting `enable_device_timeout` to `true` and configuring the timeout duration in seconds with `device_timeout_s`, which defaults to 3s.

This shall not be smaller that 2s, as the driver will only update the `Timeout` register every second or a bit more, which would lead to false positives.

If the device reports "Communication Fault Modbus" in the device fault register, this driver will try to reset the device by writing `2` to the control word 1 register. After that, the driver resets the control word 1 to the previous value.

The driver will try to write the device reset command every cycle, until the device fault is cleared.

- [ ] The device does not seem to always react to device resets, it sporadically just clears the fault, not significantly correlating to the number of reset commands sent, though some of the time it executes a reset after receiving < 3 reset commands. This needs more testing.


## Measurement and publishing modes

This driver supports three modes of operation:
- Standard mode: The device pauses measurements upon startup and when `stop()` is called. Measurements are started by calling `start()`.
- Continous measurement mode: The device contiuosly performs measurements, BIT 8 of the "control word 1" register (address 40001) is not set at any time. This is useful if the device should always alarm on isolation faults, even when no EV is connected. Measurements are still only published when `start()` is called, until `stop()` is called.
  - This mode is enabled by setting `keep_measurement_active` to `true`
- Always publish mode: Like in continous measurement mode, the device contiuosly performs measurements. Additionally, all measurements are published, even when `stop()` is called. This is only useful in very specific scenarios, e.g. if a special module needs measurements all the time
  - This mode is enabled by setting `always_publish_measurements` and `keep_measurement_active` to `true`.

## Notable device quirks

- The voltage register does not differentiate between a voltage of 0 or a invalid measurement, this means that we currently do not publish a voltage when 0 is reported.
- Self testing is only possible when Bit 8 of the "control word 1" register (address 40001) is not set, i.e. when measurement is not disabled
- When triggering a self test by setting bit 4 of the "control word 1" register (address 40001), the device can take a few seconds (about 0-2s) to publish the device state reflecting a self test.
- Alarm and pre-alarm are triggered before a lower isolation resistance is reported, and is resetted before the reported isolation resistance rises again. This is probably due to a delay in the publishing of the measured resistance.
- The device state "Error" may not always be set when a device fault is present (i.e. the device fault register shows a fault). Because of this, we only check the device fault register to determine if a fault is present and report that to everest.


## Others

- ~~As alarm is triggered before isolation resistance is updated we could publish alarm resistance threshold as measured resistance when alarm is triggered~~ We did not do this, because, when Alarmspeicherung is enabled, the reported resistance would be wrong. Also this is not clean behavior and may lead to bugs in the future.

## TODOs

- [x] Implement communication timeout
- [x] Implement alarmspeicherung setting
- [x] Implement indicator relay settings
- [ ] check if device name + other device info should be read and logged
- [ ] check if extended self test should be implemented
- [x] check if self test should fail when device errors occur during self test
  - Has been implemented. If the device fault register shows a fault, the self test is considered failed. This is also the case when a device timeout occurs
- [x] check if self test should fail when connection errors occur during self test
  - Has been implemented. Not tested yet
