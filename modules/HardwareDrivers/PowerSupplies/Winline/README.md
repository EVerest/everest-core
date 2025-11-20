# Winline Power Supply Driver

Driver module for Winline power supplies supporting the Winline CAN protocol.
This module can control multiple Winline modules using either fixed addressing
or automatic group discovery.

## Overview

This is an **EVerest Hardware Driver** module that implements the standardized
`power_supply_DC` interface. It translates EVerest's generic power supply
commands into Winline-specific CAN protocol messages, enabling seamless
integration of Winline hardware into EVerest-based charging systems.

**Key Integration Points:**

- **Implements**: `power_supply_DC` interface for standardized power supply
  control
- **Communicates**: Via CAN bus using Winline protocol
- **Provides**: Voltage/current control, telemetry, error reporting, and
  capability detection
- **Integrates**: With EVerest's error management, logging, and configuration
  systems

## Features

- **Multi-Module Support**: Control multiple Winline modules simultaneously
  with current sharing
- **Dual Operating Modes**: Fixed address mode or automatic group discovery
  mode
- **Comprehensive Error Handling**: Maps all Winline errors to standardized
  power_supply_DC framework errors
- **Serial Number Identification**: Uses register-based serial number reading
  for unique device identification
- **Thread-Safe Operation**: Robust concurrent access protection for
  telemetry data
- **Automatic Recovery**: Detects and recovers from offline modules
  automatically
- **Enhanced Status Monitoring**: Comprehensive status tracking and trend
  analysis
- **Power State Verification**: Verifies power commands with actual module
  status

## Configuration

### Basic Configuration

```yaml
# Required: CAN interface
can_device: "can0"

# Choose ONE of the following addressing modes:

# Option 1: Fixed Address Mode (recommended for known setups)
module_addresses: "0,1,2"  # Comma-separated list of module addresses

# Option 2: Group Discovery Mode (for automatic allocation)
group_address: 1           # Group number matching module settings
module_addresses: ""       # Must be empty for group discovery

# Optional settings
device_connection_timeout_s: 15    # Module offline timeout
conversion_efficiency_export: 0.95 # Power conversion efficiency
controller_address: 240            # CAN controller address
power_state_grace_period_ms: 2000  # Grace period for power state verification
altitude_setting_m: 1000           # Working altitude in meters
input_mode: "AC"                   # Input mode: "AC" or "DC"
module_current_limit_point: 1.0    # Current limit multiplier
```

### Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `can_device` | string | `"can0"` | CAN interface name (e.g., "can0", "vcan0") |
| `module_addresses` | string | `""` | Fixed addresses: "0,1,2" or empty for group discovery |
| `group_address` | integer | `0` | Group number for discovery mode |
| `device_connection_timeout_s` | integer | `15` | Module offline timeout |
| `conversion_efficiency_export` | number | `0.95` | Power conversion efficiency (0.0-1.0) |
| `controller_address` | integer | `240` | CAN controller address |
| `power_state_grace_period_ms` | integer | `2000` | Grace period for power state verification |
| `altitude_setting_m` | integer | `1000` | Working altitude in meters |
| `input_mode` | string | `"AC"` | Input mode: "AC" or "DC" |
| `module_current_limit_point` | number | `1.0` | Current limit multiplier |

## Operating Modes

### 1. Fixed Address Mode (Recommended)

Use when you know the exact addresses of your Winline modules.

**Configuration:**

```yaml
module_addresses: "0,1,2"  # List specific module addresses
group_address: 0           # Ignored in this mode
```

**Advantages:**

- Predictable, deterministic addressing
- Faster startup (no discovery phase)
- Direct control over which modules are used

**Use Cases:**

- Production installations with known hardware
- Testing with specific module configurations
- When you need guaranteed addressing consistency

### 2. Group Discovery Mode

Use when modules are configured for automatic address allocation.

**Configuration:**

```yaml
module_addresses: ""       # Must be empty
group_address: 1           # Must match module group setting
```

**Advantages:**

- Automatic module detection
- Hot-plugging support
- Easier setup for dynamic configurations
- Adapts to hardware changes

**Requirements:**

- Module group settings must match group_address
- All modules in group must be powered on during discovery

**Use Cases:**

- Development and testing environments
- Field installations with variable hardware
- When module addresses may change

## CAN Protocol Details

### Protocol Specification

- **Standard**: Winline CAN Communication Protocol
- **Bus Type**: CAN 2.0B Extended Frame Format (29-bit identifiers)
- **Baud Rate**: 125 kbps
- **Message Length**: 8 bytes (fixed)
- **Termination**: 120Ω resistors at both ends

### CAN ID Structure (29-bit)

```
Bits 28-26: Error Code (3 bits)
Bits 25-22: Device Number (4 bits)
Bits 21-16: Command Number (6 bits)
Bits 15-8:  Destination Address (8 bits)
Bits 7-0:   Source Address (8 bits) - Controller address
```

### Key Registers Used

| Register | Address | Description | Usage |
|----------|---------|-------------|-------|
| Voltage | 0x0001 | Read output voltage | Regular telemetry |
| Current | 0x0002 | Read output current | Regular telemetry |
| Status | 0x0003 | Read status flags | Error monitoring |
| Rated Power | 0x0004 | Read rated output power | Capability detection |
| Rated Current | 0x0005 | Read rated output current | Capability detection |
| Serial Low | 0x0054 | Read serial number (low) | Device identification |
| Serial High | 0x0055 | Read serial number (high) | Device identification |
| Group Info | 0x0043 | Read group information | Group discovery |
| Set Voltage | 0x0021 | Set output voltage | Control |
| Set Current | 0x001B | Set output current | Control |
| Power Control | 0x001A | Enable/disable module | Control |

### Communication Flow

#### Initialization

1. **Discovery** (Group mode): Send Group Info request to detect modules
2. **Capabilities**: Read max voltage/current/power from each module
3. **Serial Numbers**: Read serial number registers for device identification
4. **Configuration**: Set altitude, input mode, and current limit point
5. **Safety**: Ensure all modules start in OFF state

#### Regular Operation (1-second intervals)

1. **Telemetry**: Read voltage, current, status from all modules
2. **Error Monitoring**: Check status flags for alarm conditions
3. **Timeout Detection**: Remove unresponsive modules from active list
4. **Recovery**: Automatically re-add modules that come back online
5. **Power Verification**: Verify power commands with actual module status

## Error Handling

### Error Mapping

Winline errors are automatically mapped to standardized power_supply_DC errors:

| Winline Error | power_supply_DC Error | Severity | Description |
|---------------|------------------------|----------|-------------|
| OverVoltage | OverVoltageDC | High | Output voltage too high |
| UnderVoltage | UnderVoltageDC | High | Output voltage too low |
| OverTemperature | OverTemperature | High | Module overheating |
| OverCurrent | OverCurrentDC | High | Output current too high |
| InputVoltage | UnderVoltageAC | High | AC input voltage issues |
| InternalFault | HardwareFault | High | Internal module fault |
| InputPhaseLoss | VendorError | High | AC phase loss |
| FanFault | VendorWarning | Medium | Cooling fan failure |
| CommunicationFault | CommunicationFault | High | Lost communication |

### Error Recovery

- **Individual Module Errors**: Other modules continue operating
- **Communication Timeout**: Modules automatically removed from active list
- **Communication Recovery**: Modules automatically re-added when responding
- **System-Wide Faults**: All modules forced off for safety
- **Automatic Recovery**: Attempts automatic recovery for overvoltage and
  short circuit faults

### Logging Format

Error messages follow standardized format for easy identification:

```
Winline[0x00/SN_12345678]: Module fault alarm activated
Winline[0x01]: Communication fault detected, FORCE mode to off
```

## Current Sharing

When multiple modules are active, current is automatically shared equally:

**Example with 3 modules:**

- Request: 150A total
- Per-module: 50A each
- Voltage: Same for all modules

**Load Balancing:**

- Automatic equal current distribution
- Voltage synchronized across all modules
- Individual module limits respected
- Failed modules automatically excluded

## Troubleshooting

### Common Issues

#### 1. No modules detected

**Symptoms**: "No active modules" warnings
**Causes**:

- CAN interface down: `sudo ip link set can0 up type can bitrate 125000`
- Wrong CAN device name: Check `can_device` configuration
- Modules not powered or wrong group setting

**Solutions**:

```bash
# Check CAN interface
ip link show can0

# Bring up CAN interface
sudo ip link set can0 up type can bitrate 125000

# Monitor CAN traffic
candump can0
```

#### 2. Modules going offline

**Symptoms**: "module communication expired" messages
**Causes**:

- CAN bus errors or noise
- Loose connections
- Power supply issues

**Solutions**:

- Check CAN bus termination (120Ω resistors)
- Verify all connections are secure
- Monitor for CAN errors in system logs

#### 3. Current not shared properly

**Symptoms**: Uneven current distribution
**Causes**:

- Modules have different capabilities
- Some modules in fault state
- Configuration mismatch

**Solutions**:

- Check module status in logs
- Verify all modules have same specifications
- Check for error conditions

#### 4. Group discovery not working

**Symptoms**: No modules found in group mode
**Causes**:

- Wrong group_address setting
- Module group settings not matching
- Modules not ready during discovery

**Solutions**:

- Verify module group settings match group_address
- Ensure all modules powered on before starting

### Debug Information

Enable debug logging to see detailed protocol information:

```yaml
# In your EVerest configuration
logging:
  Winline: debug
```

Monitor CAN traffic:

```bash
# Raw CAN monitoring
candump can0

# With timestamp
candump -t z can0

# Filter Winline traffic (controller address 0xF0)
candump can0 | grep F0
```

## Hardware Requirements

### CAN Bus Setup

- **Topology**: Linear bus with 120Ω termination at both ends
- **Cable**: Twisted pair CAN cable (CAN_H, CAN_L)
- **Max Distance**: 40m at 125 kbps
- **Max Devices**: Multiple Winline modules + 1 controller

### Module Configuration

- Set module addresses or group settings before powering on
- Ensure all modules use same firmware version
- Configure appropriate current/voltage limits
- Set correct altitude and input mode

## Development Notes

### Architecture Constraints (Critical Implementation Details)

#### Single-Threaded CAN Communication

**IMPORTANT**: The entire CAN communication system operates in a single thread:

- `rx_handler()` and `poll_status_handler()` execute in the same thread
- No concurrent CAN operations - all message processing is sequential
- This design ensures deterministic message handling and eliminates race
  conditions
- **Implication**: Blocking operations in handlers affect entire CAN
  communication

#### Configuration Immutability

**CRITICAL**: Configuration is set exactly once during initialization:

- `set_config_values()` called only once at startup
- No runtime configuration changes supported
- Module addresses and operating mode cannot be changed after initialization
- **Implication**: Configuration errors require full module restart

### Security Model

#### No CAN Bus Security

**IMPORTANT**: Winline CAN protocol has no built-in security:

- **No encryption**: All CAN messages transmitted in plain text
- **No authentication**: No verification of message sender identity
- **No message integrity**: Only basic CRC provided by CAN hardware
- **Physical security required**: Secure the CAN bus physically

#### Message Validation Strategy

**Minimal Validation by Design**:

- **Only validation**: Controller address verification
- **Only malformation check**: Message must be exactly 8 bytes
- **No payload validation**: Content validity checking beyond packet structure
- **Rationale**: Keep protocol simple and fast for real-time operation

### Adding New Commands

1. Define register constants in `CanPackets.hpp`
2. Implement packet structures in `CanPackets.cpp`
3. Add command handling in `WinlineCanDevice::rx_handler()`
4. Add sending method if needed

### Thread Safety

- `telemetries` map: Protected by implicit single-thread access
- `active_module_addresses`: Protected by `active_modules_mutex`
- All signals are thread-safe via sigslot library

### Testing

- Use `vcan0` virtual CAN interface for development
- Mock modules can be simulated with `cansend` commands
- Unit tests recommended for packet encoding/decoding

## References

- [EVerest power_supply_DC Interface](https://everest.github.io/) - Standardized interface documentation
- [Linux SocketCAN Documentation](https://www.kernel.org/doc/html/latest/networking/can.html) - CAN bus programming guide

## Hardware Compatibility

### Supported Winline Models

- **Protocol Version**: Winline protocol and compatible versions
- **Module Types**: Various Winline power supply modules

### Manufacturer Information

- **Manufacturer**: Winline
- **Protocol Version**: Winline protocol V1.50 (this driver implementation)

**Note**: Ensure your Winline modules support the protocol version used by this driver. Older protocol versions may not be fully compatible.
