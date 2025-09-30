# Dold RN5893 Isolation Monitor

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


## Others

- ~~As alarm is triggered before isolation resistance is updated we could publish alarm resistance threshold as measured resistance when alarm is triggered~~ We did not do this, because, when Alarmspeicherung is enabled, the reported resistance would be wrong. Also this is not clean behavior and may lead to bugs in the future.

## TODOs

- [ ] Implement communication timeout
- [x] Implement alarmspeicherung setting
- [ ] Implement indicator relay settings
- [ ] check if device name + other device info should be read and logged
- [ ] Test error handling
  - [ ] Modbus disconnect at arbitrary times
  - [ ] Device fault handling
  - [ ] Communication timeout (see first todo)
