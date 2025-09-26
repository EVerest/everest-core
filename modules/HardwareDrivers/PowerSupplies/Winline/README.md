# Winline Power Supply Driver

## 🔧 Next Steps for Customization

This module is now ready for CAN protocol adaptation. To implement your Winline-specific protocol:

### 1. Update Protocol Implementation (REQUIRED)
- [ ] **Edit `can_driver_acdc/CanPackets.hpp`**: Replace all WinlineProtocol constants with your actual protocol values
- [ ] **Edit `can_driver_acdc/CanPackets.cpp`**: Update CAN ID encoding functions and packet implementations
- [ ] **Edit `can_driver_acdc/WinlineCanDevice.cpp`**: Update rx_handler logic for your specific protocol
- [ ] **Edit `main/power_supply_DCImpl.cpp`**: Update error mapping function for your error types

### 2. Update Configuration (REQUIRED)
- [ ] **Edit `manifest.yaml`**: Update description, parameters, and metadata for your hardware
- [ ] **Test configuration**: Verify all parameters work with your Winline hardware

### 3. Protocol Customization Areas

**Key files prepared for protocol changes:**

| File | What to Change |
|------|----------------|
| `CanPackets.hpp` | Protocol constants in `WinlineProtocol` namespace |
| `CanPackets.cpp` | CAN ID encoding/decoding functions |
| `WinlineCanDevice.hpp` | Error enum and telemetry structure |
| `WinlineCanDevice.cpp` | Message handling and protocol logic |

### 4. Current State

**✅ Completed:**
- Module renamed from InfyPower to Winline
- All class names updated (WinlineCanDevice, etc.)
- All logging prefixes changed to "Winline:"
- Build system updated (CMakeLists.txt)
- EVerest integration maintained

**🔧 Ready for protocol customization:**
- CAN packet structures (replace with your protocol)
- Protocol constants (update with your values)
- Error mapping (adapt to your error types)
- Discovery mechanism (adapt to your method)

### 5. Testing Checklist
- [ ] **Build verification**: Ensure module compiles
- [ ] **Virtual CAN**: Test basic CAN communication with `vcan0`
- [ ] **Protocol validation**: Verify CAN ID encoding/decoding with your protocol
- [ ] **Error handling**: Test all error conditions
- [ ] **Multi-module**: Test with multiple modules if supported
- [ ] **EVerest integration**: Verify power_supply_DC interface compliance

## Configuration Example

```yaml
# Current Winline configuration (update for your protocol)
can_device: "can0"
module_addresses: "0,1,2"  # Update addressing scheme if needed
group_address: 1           # Update for your discovery mechanism
device_connection_timeout_s: 15  # Update based on your protocol timing
conversion_efficiency_export: 0.95
controller_address: 240    # Update for your protocol
```

## Development Notes

- **Based on**: InfyPower template - proven, production-ready foundation
- **Architecture**: Single-threaded CAN communication with EVerest integration
- **Thread Safety**: Mutex protection for shared data structures
- **Error Handling**: Comprehensive error mapping to EVerest standard errors
- **Protocol Ready**: Structure prepared for easy protocol adaptation

## Original Template

This driver maintains the same proven architecture as the InfyPower driver:
- Robust CAN communication handling
- Comprehensive error management
- Multi-module support with current sharing
- EVerest framework integration
- Thread-safe operation

**Ready for your Winline protocol implementation!**

---
*Generated from InfyPower template - customize for your Winline protocol*