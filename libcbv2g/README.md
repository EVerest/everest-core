# libcbV2G - The V2GTP EXI codec library

libcbv2g is a library to encode and decode EXI messages and is able to process DIN70121, ISO15118-2 and ISO15118-20 messages. The library is based on the generated code output of the [cbExiGen](https://github.com/Everest/cbexigen) generator.

All documentation and the issue tracking can be found in our main repository here: https://github.com/EVerest/everest

## Dependencies

To build this library you need [everest-cmake](https://github.com/EVerest/everest-cmake) checkout in the same directory as libcbV2G.

## Getting started

```
# Run cmake (CB_V2G_BUILD_TESTS to enable/disable unit tests)
cmake -S . -B build -G Ninja -DCB_V2G_BUILD_TESTS=1 -DCMAKE_EXPORT_COMPILE_COMMANDS=1

# Build
ninja -C build

# Running tests
ninja -C build test
```
