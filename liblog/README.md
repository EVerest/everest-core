# C++ logging and exceptions library for the EVerest framework

Provides a common infrastructure for all EVerest modules on logging, wrapped around Boost.Log.

All documentation and the issue tracking can be found in our main repository here: https://github.com/EVerest/everest

## Build instructions
==================

Clone the repository:

```bash
  git clone https://github.com/EVerest/liblog
```

Create a folder named build and cd into it.
Execute cmake and then make install:

```bash
  mkdir build && cd build
  cmake ..
  make install
```

To check your code with clang-tidy you can use the following cmake
command:

```
  cmake .. -DCMAKE_RUN_CLANG_TIDY=ON make
```

To run unit tests and generate a code coverage report you can run the
following commands:

```
  make && make install && make gcovr_coverage
```

The coverage report will be available in the index.html file in the
```build/gcovr_coverage``` directory.
