ISO 15118 library suite
=======================

Currently, this is a proof of concept library.

Integration of ISO 15118 development for EVerest is under heavy development
currently. Many features are already implemented, many are still to follow.

But: This repository here is not the place where implementation happens.
To see current development work, have a look at those places in EVerest:

- Some functionality of part 2 of ISO 15118 is integrated in the
  [EvseManager module in the everest-core repository](https://github.com/EVerest/everest-core/tree/main/modules/EvseManager).
- Current development for an EXI code generator (as used in the
  ISO 15118 protocol suite) is ongoing in the
  [cbexigen repository](https://github.com/EVerest/cbexigen). This will
  make the currently used ext-openv2g repo obsolete.
- The [repository libSlac](https://github.com/EVerest/libslac) contains
  definitions of SLAC messages that are used for ISO 15118 communication.
- V2G-related funcationality can be found in
  [EVerest module EvseV2G](https://github.com/EVerest/everest-core/tree/main/modules/EvseV2G)

This is just a short overview to show you that there is not the one and
only place for the ISO 15118 implementation. For more information and
insights on that, join us in our
[weekly tech meetups](https://everest.github.io/nightly/#weekly-tech-meetup).

## Dependencies

To build this library you need [everest-cmake](https://github.com/EVerest/everest-cmake) checkout in the same directory as libiso15118.

## Getting started

```
# Run cmake (ISO15118_BUILD_TESTS to enable/disable unit tests)
cmake -S . -B build -G Ninja -DISO15118_BUILD_TESTS=1 -DCMAKE_EXPORT_COMPILE_COMMANDS=1

# Build
ninja -C build

# Running tests
ninja -C build test
```
