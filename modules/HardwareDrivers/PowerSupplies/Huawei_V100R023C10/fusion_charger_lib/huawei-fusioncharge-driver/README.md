# Huawei Fusion Charger Driver

## Build and test

This library is built and tested as part of the build process of everest-core.

## Run real_hw_first_test on fricklydevnuc3

```bash
cd build/modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/fusion_charger_lib/huawei-fusioncharge-driver/examples
sudo ./examples/real_hw_first_test 192.168.11.1 502 enp86s0
```

## Libs

- `fusion_charger_modbus_extensions` modbus extension for fusion charger (primarily unsolicitated reports)
- `fusion_charger_modbus_driver` modbus driver stuff for fusion charger
