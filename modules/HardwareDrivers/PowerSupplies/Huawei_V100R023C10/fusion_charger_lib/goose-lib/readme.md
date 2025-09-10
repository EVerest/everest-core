# Goose library

Provides:
- Ethernet interface for macOS and Linux
  - note that macOS implementation is not really tested; Linux's implementation is mostly based on Huawei's FusionCharger documentation
- Ethernet frame abstraction
- Goose Frame abstraction
  - contains Goose PDU abstraction which in turn contains APDU BER encoded data, which is partly abstracted

## Build and test

### Build without tests

```bash
cmake -B build
cd build
make -j$(nproc)
```

### Build with tests

```bash
cmake -B build -DBUILD_TESTING=ON
cd build
make -j$(nproc)
```

### Run tests

```bash
cd build
ctest
```