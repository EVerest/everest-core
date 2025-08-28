# Huawei V100R023C10 PSU

## Running the SIL

### Single connector SIL

To run the single connector SIL, the following commands are needed to create the virtual ethernet interface:

```bash
sudo ip link add veth0 type veth peer name veth1
sudo ip link set dev veth0 up
sudo ip link set dev veth1 up
```

After that, run the SIL in one terminal: `sudo -E ./run-scripts/run-huawei-fusion-charge-sil.sh`

In another Terminal start the power supply mock: `sudo ./modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/fusion_charger_lib/fusion-charger-dispenser-library/power_stack_mock/fusion_charger_mock`

In a third terminal start NodeRed: `./run-scripts/nodered-sil-dc.sh`

### Dual Connector SIL

In addition to the `veth` creation process for the single connector SIL run the following commands, as they are needed to simulate the iso15118 between the two virtual cars:

```bash
sudo ip link add veth00 type veth peer name veth01
sudo ip link set dev veth00 up
sudo ip link set dev veth01 up
sudo ip link add veth10 type veth peer name veth11
sudo ip link set dev veth10 up
sudo ip link set dev veth11 up
```

After that, run the dual SIL in one terminal: `sudo -E ./run-scripts/run-huawei-fusion-charge-sil-dual.sh`

In another Terminal start the power supply mock: `sudo ./modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/fusion_charger_lib/fusion-charger-dispenser-library/power_stack_mock/fusion_charger_mock`

In a third terminal start NodeRed: `./run-scripts/nodered-sil-dc-dual.sh`


### Quad Connector SIL

In addition to the `veth` creation process for the single and dual connector SILs run the following commands:

```bash
sudo ip link add veth20 type veth peer name veth21
sudo ip link set dev veth20 up
sudo ip link set dev veth21 up
sudo ip link add veth30 type veth peer name veth31
sudo ip link set dev veth30 up
sudo ip link set dev veth31 up
```

After that, run the dual SIL in one terminal: `sudo -E ./run-scripts/run-huawei-fusion-charge-sil-quad.sh`

In another Terminal start the power supply mock: `sudo ./modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/fusion_charger_lib/fusion-charger-dispenser-library/power_stack_mock/fusion_charger_mock`

In a third terminal start NodeRed: `./run-scripts/nodered-sil-dc-quad.sh`
