C++ timer library for the EVerest framework
===========================================
EVerest is aiming to enable logfile replay. All EVerest components have to utilize this library for time (and delay) related things in order to enable accelerated logfile replay speed in the future.

All documentation and the issue tracking can be found in our main repository here: https://github.com/EVerest/everest


Prerequisites
=============

Dependencies (Ubuntu)
---------------------------

On Ubuntu all other dependencies can be installed with the advanced
packaging tool:

```
  sudo apt install build-essential libboost-all-dev gcovr lcov
```

Cmake 3.14.7 (or higher)
------------------------

Visit <https://cmake.org/download/> and download the latest version of
cmake, then install it.

### Linux

Download the corresponding .sh-script for your architecture (x86\_64 or
aarch64). Move the downloaded file to ```/opt``` and make it executable.

```
  sudo mv cmake-3.*version*.sh /opt
  sudo chmod +x cmake-3.*version*.sh
```

Run the script and press ‚y‘ twice. The script will then install cmake.

```
  sudo ./cmake-3.*version*.sh
```

In order to use the new installed cmake version instead of the old one
you have to create a symbolic link:

```
  sudo ln -s /opt/cmake-3.*version*/bin/* /usr/local/bin
```

To see if this whole procedure was successful, type:

```
  cmake --version
```

It should now show the fresh installed version of cmake.

Build instructions
==================

Clone the repository:

```
  git clone https://github.com/EVerest/time
```

Create a folder named build and cd into it.
Execute cmake and then make install:

```
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
