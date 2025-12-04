# Dold RN5893 Isolation Monitor

## Self test

The Dold RN5893 supports two types of self tests: the normal self test and the extended self test.
This module only supports the "normal" self tests, not the extended self test.

When a self test is started via the IMD interface, bit 4 of the "control word 1" register (address 40001) is set.
The device will then perform a self test. If a device fault is present while the self test is running, the self test will fail.
If the device reports a state that is not "self test running" anymore and there is no device fault, the self test is considered successful.

Changes of the device state, including self tests, are reported with a short delay via Modbus. This is handled internally using the `self_test_running` and `self_test_triggered` variables of the driver.

## Timeout

The device supports a communication timeout, which raises a device fault if no communication is possible for a certain time.

This module supports this timeout. It is enabled by setting `timeout_release` to `true` and optionally configuring the timeout duration in seconds using `timeout_s`, which defaults to 3s.

This value should not be smaller than 2s, as the driver updates the *Timeout* register only once per second (or slightly less frequently), which would otherwise lead to false positives.

If the device reports *Communication Fault Modbus* in the *device fault* register, the driver will try to reset the device by writing `1` to the *control word 1* register. After that, the driver resets the control word 1 to the previous value.

The driver will try to write the device reset command every cycle until the device fault is cleared.

## Measurement and publishing modes

This driver supports three modes of operation:
- Standard mode: The device pauses measurements upon startup and when `stop()` is called. Measurements are started by calling `start()`
- Continous measurement mode: The device continuously performs measurements, bit 8 of the "control word 1" register (address 40001) is not set at any time. This is useful if the device should always alarm on isolation faults, even when no EV is connected. Measurements are still only published when `start()` is called, until `stop()` is called
  - This mode is enabled by setting `keep_measurement_active` to `true`
- Always publish mode: Like in continous measurement mode, the device continuously performs measurements. Additionally, all measurements are published, even when `stop()` is called. This is only useful in very specific scenarios, e.g. if a special module needs measurements all the time
  - This mode is enabled by setting both `always_publish_measurements` and `keep_measurement_active` to `true`

## Other notes

- For applications following UL 2231 the parameter `automatic_self_test` has to be disabled (i.e. set to `false`)
- Self testing is only possible when bit 8 of the "control word 1" register (address 40001) is not set, i.e. when measurement is not disabled. Because of this, the module enables measurements temporarily before starting a self test
- Changes to modbus registers triggered by modbus messages may have a short delay before they can be read back over the bus. The device's internal reaction is faster than what is reported via Modbus, however
- The device state may not always report "Error" when a device fault is present (i.e. the device has an internal fault or the device fault register reports a fault), because of internal prioritization of faults. Because of this, we only check the device fault register to determine if a fault is present and report that to Everest
- If a modbus communication timeout occurs, the device only responds to modbus requests that either read data or write a reset command. All other requests will receive a 0x04 exception code
- The voltage register does not differentiate between a voltage of 0 or an invalid measurement, this means that we do not publish a voltage when 0 is reported
