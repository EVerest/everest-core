# Fusion Charger Dispenser Lib

A Library that provides a high-level interface for the Huawei Fusion-Charge Power-Suply Unit

## Create virtual ethernet interface

```bash
sudo ip link add veth0 type veth peer name veth1
sudo ip link set dev veth0 up
sudo ip link set dev veth1 up
```

### Delete again
```bash
sudo ip link delete veth0
```

## FSM

```mermaid
stateDiagram
  [*] --> CarDisconnected

  state CarConnected {
    [*] --> NoKeyYet
    NoKeyYet
    ConnectedNoAllocation
    Running
    Completed
  }

  CarDisconnected --> CarConnected : Car Connected

  CarConnected --> CarDisconnected : Car Disconnected

  NoKeyYet --> ConnectedNoAllocation : HMAC Key received
  ConnectedNoAllocation --> Running : Module allocation received or Timeout
  state Running {
    ExportCablecheck
    OffCablecheck
    ExportPrecharge
    ExportCharging
    ExportCharging
  }
  note right of Running
    All transitions in Running are possible, they are dependent on the EV Mode and EV Phase (see below). To exit Running a Off Mode in a non Cablecheck phase is required.
  end note

  Running --> Completed : Mode Off / [!Cablecheck]
  Completed --> NoKeyYet : Mode Export
```

### Detailed States

#### CarDisconnected

The car is not connected to the charger, we send stop goose frames and report an **Standby** working state

#### NoKeyYet

The car is now connected to the charger but we don't have a hmac key yet. We set the working state to **StandbyWithConnectorInserted** which will trigger a hmac key generation on the PSU (which we receive via modbus). We still send stop goose frames.

#### ConnectedNoAllocation

After we receive the hmac key via modbus we send placeholder requests to the PSU. To do that we have to be in the **ChargingStarting** working state. If we don't receive a response from the PSU in time we will still go to the Running state (as the goose answer frame might just have been dropped and we will not know that). If we receive a response we go to the Running state.

#### Running

We now have successfully allocated a module on the PSU and have a HMAC key thus we can start charging!

Initially we still send placeholder requests to the PSU and are in the ChargingStarting working State.

After a while everest will report some other Mode and phase. Depending on the mode and phase we go to different working states (see below).

When we receive an Off without being in the Cablecheck phase we go to the Completed state.

#### Completed

We are done charging and ready to charge again. If the car disconnects we go back to CarDisconnected; if we receive another Export mode we go back to NoKeyYet and acquire a new key and module placeholder allocation

### Workingstates

| State (siehe oben)    | EV Mode | EV Phase   | Working state                |
| --------------------- | ------- | ---------- | ---------------------------- |
| CarDisconnected       | *       | *          | Standby                      |
| NoKeyYet              | *       | *          | StandbyWithConnectorInserted |
| ConnectedNoAllocation | *       | *          | ChargingStarting             |
| Running               | Off     | *          | ChargingStarting             |
| Running               | Export  | Cablecheck | ChargingStarting             |
| Running               | Export  | Precharge  | ChargingStarting             |
| Running               | Export  | Charge     | Charging                     |
| Completed             | *       | *          | ChargingCompleted            |

### Goose Frames

| State (see above)     | EV Mode | EV Phase       | Goose type       | Goose PowerRequirement type              |
| --------------------- | ------- | -------------- | ---------------- | ---------------------------------------- |
| CarDisconnected       | *       | *              | Stop             |                                          |
| NoKeyYet              | *       | *              | Stop             |                                          |
| ConnectedNoAllocation | *       | *              | PowerRequirement | ModulePlaceholderRequest                 |
| Running               | Off     | * (low weight) | PowerRequirement | ModulePlaceholderRequest                 |
| Running               | Export  | Cablecheck     | PowerRequirement | InsulationDetectionVoltageOutput         |
| Running               | Off     | Cablecheck     | PowerRequirement | InsulationDetectionVoltageOutputStoppage |
| Running               | Export  | Precharge      | PowerRequirement | Precharge                                |
| Running               | Export  | Charge         | PowerRequirement | RequirementDuringCharging                |
| Completed             | *       | *              | Stop             |                                          |


### Connection State

| State(see above) | ConnectionState |
| ---------------- | --------------- |
| CarDisconnected  | NOT_CONNECTED   |
| *                | FULLY_CONNECTED |

### Charge Event

- Transition to Running: STOP_TO_START
- Transition to Completed: START_TO_STOP

### Module Placeholder Allocation failed

When we get the response from the PSU that the module placeholder allocation failed we go to Running thus we store a flag whether the module placeholder allocation was successful or not.

This flag is reset when we go out of the Running state.

### Connection state

The Module keeps track of the connection state with the PSU. There are the following states:

- `UNINITIALIZED` The Dispenser has not been started yet or has been stopped. No communication is happening
- `INITIALIZING` `start()` has been called, a modbus connection is being established. The communication is not yet ready
- `READY` The PSU's Ethernet MAC was received, communication is ready and working
- `FAILED` The communication failed, no communication is happening in this state; the module stays in this state until `stop()` is called