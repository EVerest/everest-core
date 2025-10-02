# InfyPower ACDC Power Supply Driver

Driver module for InfyPower ACDC power supplies supporting the V1.13 CAN protocol. This module can control multiple InfyPower modules (up to 60 modules theoretically, **recommended maximum 4-6 devices** for reliable operation, typically 2-6 in field installations) using either fixed addressing or automatic group discovery.

## Overview

This is an **EVerest Hardware Driver** module that implements the standardized `power_supply_DC` interface. It translates EVerest's generic power supply commands into InfyPower-specific CAN protocol messages, enabling seamless integration of InfyPower hardware into EVerest-based charging systems.

**Key Integration Points:**
- **Implements**: `power_supply_DC` interface for standardized power supply control
- **Communicates**: Via CAN bus using InfyPower V1.13 protocol
- **Provides**: Voltage/current control, telemetry, error reporting, and capability detection
- **Integrates**: With EVerest's error management, logging, and configuration systems

## Features

- **Multi-Module Support**: Control multiple InfyPower modules simultaneously with current sharing (up to 60 modules theoretically)
- **Dual Operating Modes**: Fixed address mode or automatic group discovery mode
- **Comprehensive Error Handling**: Maps all InfyPower errors to standardized power_supply_DC framework errors
- **Serial Number Identification**: Uses Command 0x0B barcode reading for unique device identification
- **Thread-Safe Operation**: Robust concurrent access protection for telemetry data
- **Automatic Recovery**: Detects and recovers from offline modules automatically

## Configuration

### Basic Configuration

```yaml
# Required: CAN interface
can_device: "can0"

# Choose ONE of the following addressing modes:

# Option 1: Fixed Address Mode (recommended for known setups)
module_addresses: "0,1,2"  # Comma-separated list of module addresses

# Option 2: Group Discovery Mode (for automatic allocation)
group_address: 1           # Group number matching PSU dial settings
module_addresses: ""       # Must be empty for group discovery

# Optional settings
device_connection_timeout_s: 10    # Module offline timeout
conversion_efficiency_export: 0.95 # Power conversion efficiency
controller_address: 240            # CAN controller address (0xF0)
```

### Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `can_device` | string | `"can0"` | CAN interface name (e.g., "can0", "vcan0") |
| `module_addresses` | string | `""` | Fixed addresses: "0,1,2" or empty for group discovery |
| `group_address` | integer | `0` | Group number for discovery mode (matches PSU dial) |
| `device_connection_timeout_s` | integer | `15` | **CRITICAL**: Must be > 10s module timeout (see Safety Notes) |
| `conversion_efficiency_export` | number | `0.95` | Power conversion efficiency (0.0-1.0) |
| `controller_address` | integer | `240` | CAN controller address (0xF0 = 240 decimal) |

## ⚠️ Safety Notes

### Critical Timeout Configuration

**IMPORTANT**: The `device_connection_timeout_s` parameter has critical safety implications and must be configured correctly.

#### InfyPower Module Internal Timeout
Per InfyPower V1.13 protocol specification: *"If the charger module did not receive the message for 10 seconds from the controller, the charger module will off."*

This means each module has a **10-second internal timeout** and will **automatically turn OFF its power output** if it doesn't receive CAN messages within this period.

#### EVerest Driver Timeout Requirement
The EVerest `device_connection_timeout_s` **MUST be longer** than the module's internal timeout. Here's why:

**❌ DANGEROUS - Driver timeout < Module timeout:**
```
Time: 0s → 8s → 10s → 12s
Driver: ✓ → ✓ → ✗ (removes module from capabilities) → ✗
Module: ✓ → ✓ → ✓ (still delivering power!) → ✗ (finally turns off)
```
**Problem**: Driver advertises reduced capability while module still delivers full power → **overcurrent risk**

**✅ SAFE - Driver timeout > Module timeout:**
```
Time: 0s → 8s → 10s → 12s → 15s
Driver: ✓ → ✓ → ✓ (still includes in capabilities) → ✓ → ✗ (removes module)
Module: ✓ → ✓ → ✗ (safely turned off) → ✗ → ✗
```
**Result**: Driver keeps module in capabilities until certain it's OFF → **no overcurrent risk**

#### Configuration Recommendation
```yaml
# InfyPower modules have 10s internal timeout (per V1.13 protocol):
device_connection_timeout_s: 15  # Safe default: 15s > 10s module timeout ✓
device_connection_timeout_s: 11  # Minimum safe: 11s > 10s module timeout

# NEVER configure equal to or shorter than module timeout:
device_connection_timeout_s: 10  # DANGEROUS: Equal to module timeout
device_connection_timeout_s: 8   # DANGEROUS: 8s < 10s module timeout
```

#### Safety Principle
- **Overestimating capabilities is safe**: System delivers less than advertised
- **Underestimating capabilities is dangerous**: Could cause overcurrent conditions
- **Always err on the side of longer timeouts** for safety

## Operating Modes

### 1. Fixed Address Mode (Recommended)

Use when you know the exact addresses of your InfyPower modules.

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
group_address: 1           # Must match PSU group dial setting
```

**Advantages:**
- Automatic module detection
- Hot-plugging support
- Easier setup for dynamic configurations
- Adapts to hardware changes

**Requirements:**
- PSU dial switches must be set to matching group number
- All modules in group must be powered on during discovery

**Use Cases:**
- Development and testing environments
- Field installations with variable hardware
- When module addresses may change

## CAN Protocol Details

### Protocol Specification
- **Standard**: InfyPower V1.13 CAN Communication Protocol
- **Bus Type**: CAN 2.0B Extended Frame Format (29-bit identifiers)
- **Baud Rate**: 125 kbps
- **Message Length**: 8 bytes (fixed)
- **Termination**: 120Ω resistors at both ends

### CAN ID Structure (29-bit)
```
Bits 28-26: Error Code (3 bits)
Bits 25-22: Device Number (4 bits) - 0x0A=single, 0x0B=group
Bits 21-16: Command Number (6 bits)
Bits 15-8:  Destination Address (8 bits)
Bits 7-0:   Source Address (8 bits) - Controller address
```

### Key Commands Used
| Command | ID | Description | Usage |
|---------|----|---------|----|
| Module Count | 0x02 | Get number of modules in group | Group discovery |
| Module VI | 0x03 | Read voltage/current | Regular telemetry |
| Module Status | 0x04 | Read status flags | Error monitoring |
| Module Capabilities | 0x0A | Read voltage/current/power limits | Capability detection |
| Module Barcode | 0x0B | Read serial number | Device identification |
| Module VI After Diode | 0x0C | Read external voltage/available current | Advanced telemetry |
| Set Module On/Off | 0x1A | Enable/disable module | Control |
| Set Module VI | 0x1C | Set voltage/current | Control |

### Communication Flow

#### Initialization
1. **Discovery** (Group mode): Send Module Count request to detect modules
2. **Capabilities**: Read max voltage/current/power from each module
3. **Serial Numbers**: Read barcode for device identification
4. **Safety**: Ensure all modules start in OFF state

#### Regular Operation (1-second intervals)
1. **Telemetry**: Read voltage, current, status from all modules
2. **Error Monitoring**: Check status flags for alarm conditions
3. **Timeout Detection**: Remove unresponsive modules from active list
4. **Recovery**: Automatically re-add modules that come back online

## Error Handling

### Error Mapping

InfyPower errors are automatically mapped to standardized power_supply_DC errors:

| InfyPower Error | power_supply_DC Error | Severity | Description |
|-----------------|----------------------|----------|-------------|
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

### Logging Format

Error messages follow standardized format for easy identification:
```
Infy[0x00/SN_130C15A3FB06A8_V]: Module fault alarm activated
Infy[0x01]: Communication fault detected, FORCE mode to off
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

#### 0. ⚠️ SAFETY: Always check timeout configuration first
**Before troubleshooting**: Verify `device_connection_timeout_s` > module internal timeout (see Safety Notes above)

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
- PSU dial switches not matching
- Modules not ready during discovery

**Solutions**:
- Verify PSU dial switch settings match group_address
- Ensure all modules powered on before starting

### Debug Information

Enable debug logging to see detailed protocol information:
```yaml
# In your EVerest configuration
logging:
  InfyPower: debug
```

Monitor CAN traffic:
```bash
# Raw CAN monitoring
candump can0

# With timestamp
candump -t z can0

# Filter InfyPower traffic (controller address 0xF0)
candump can0 | grep F0
```

## Hardware Requirements

### CAN Bus Setup
- **Topology**: Linear bus with 120Ω termination at both ends
- **Cable**: Twisted pair CAN cable (CAN_H, CAN_L)
- **Max Distance**: 40m at 125 kbps
- **Max Devices**: Up to 60 InfyPower modules + 1 controller (typically 2-6 in field installations)

### PSU Configuration
- Set module addresses or group dial switches before powering on
- Ensure all modules use same firmware version
- Configure appropriate current/voltage limits

## Development Notes

### Architecture Constraints (Critical Implementation Details)

#### Single-Threaded CAN Communication
**IMPORTANT**: The entire CAN communication system operates in a single thread:
- `rx_handler()` and `poll_status_handler()` execute in the same thread
- No concurrent CAN operations - all message processing is sequential
- This design ensures deterministic message handling and eliminates race conditions
- **Implication**: Blocking operations in handlers affect entire CAN communication

#### Configuration Immutability
**CRITICAL**: Configuration is set exactly once during initialization:
- `set_config_values()` called only once at startup
- No runtime configuration changes supported
- Module addresses and operating mode cannot be changed after initialization
- **Implication**: Configuration errors require full module restart

### Security Model

#### No CAN Bus Security
**IMPORTANT**: InfyPower CAN protocol has no built-in security:
- **No encryption**: All CAN messages transmitted in plain text
- **No authentication**: No verification of message sender identity
- **No message integrity**: Only basic CRC provided by CAN hardware
- **Physical security required**: Secure the CAN bus physically

#### Message Validation Strategy
**Minimal Validation by Design**:
- **Only validation**: Controller address verification (`destination_address == controller_address`)
- **Only malformation check**: Message must be exactly 8 bytes
- **No payload validation**: Content validity checking beyond packet structure
- **Rationale**: Keep protocol simple and fast for real-time operation

### Adding New Commands
1. Define packet structure in `CanPackets.hpp`
2. Implement constructor and conversion in `CanPackets.cpp`
3. Add command handling in `InfyCanDevice::rx_handler()`
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

- [InfyPower V1.13 CAN Protocol Documentation](https://drive.google.com/file/d/1bZ9Pr7JD_ichvKmLmh8eVnZi0CdSr0xF/view?usp=drive_link) - Complete protocol specification
- [EVerest power_supply_DC Interface](https://everest.github.io/) - Standardized interface documentation
- [Linux SocketCAN Documentation](https://www.kernel.org/doc/html/latest/networking/can.html) - CAN bus programming guide

## Hardware Compatibility

### Supported InfyPower Models
- **REG Series**: REG1K0100G and compatible models
- **CEG Series**: CEG1K0100G and compatible models (includes MPPT mode support)
- **LRG Series**: LRG series charger modules
- **Protocol Version**: V1.13 and compatible versions

### Manufacturer Information
- **Manufacturer**: Shenzhen Infypower Co., Ltd
- **Website**: http://www.infypower.com
- **Protocol Version**: V1.13 (this driver implementation)
- **Document Reference**: "REG/CEG/LRG series Charger Module CAN Communication Protocol V1.13"

**Note**: Ensure your InfyPower modules support V1.13 protocol or compatible versions. Older protocol versions may not be fully compatible with this driver.
