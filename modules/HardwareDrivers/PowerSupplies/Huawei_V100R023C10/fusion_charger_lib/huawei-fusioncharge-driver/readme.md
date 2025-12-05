# Huawei Fusion Charger Driver

## Build

```bash
cmake -B build
cd build
make -j$(nproc)
```

## Run real_hw_first_test on fricklydevnuc3

```bash
cd build
sudo ./examples/real_hw_first_test 192.168.11.1 502 enp86s0
```


## Libs

- `fusion_charger_modbus_extensions` modbus extension for fusion charger (primarily unsolicitated reports)
- `fusion_charger_modbus_driver` modbus driver stuff for fusion charger
