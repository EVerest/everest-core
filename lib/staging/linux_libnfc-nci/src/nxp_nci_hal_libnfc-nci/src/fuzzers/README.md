# Summary

This directory contains a couple NFC fuzzers targeting different part of the
NFC code. Due to the complexity of NFC codebase, it's not easy to directly
fuzzing an E2E NFC scenario. Instead, it's much easier to simply target a
specific function which does the packet parsing, and link only related code
with some external dependencies stubbed out. That's how these fuzzers are
created.


* nci:
NCI packet parsing code -- vulnerabilites found in this fuzzer usually require
NFC firmware compromise.

* rw:
NFC tag Read/Write code -- vulnerabilities found here are possibly reachable by
an attacker countrolled NFC tag or tag emulator.

* ce:
NFC tag emulation code -- vulnerabilities found here are possibly reachable by
an attacker controlled NFC tag reader.


# Running fuzzers

Here are the steps to run these fuzzers locally. These steps are verified with
an engineering Android device, and my linux workstation with all the necessary
tools installed:

1. Getting an Android device capable of HWASAN

    I've been using Pixel 3 (Blueline) and it works great. So we will use
    "blueline" for the rest of the steps. If you are using a different device,
    you need to adjust the command accordingly.

2. Preparing the device:

    You need to flash the device with a HWASAN userdebug build. For a blueline
    device, it will be "blueline_hwasan-userdebug". After that, make sure the
    device connects to your workstation and adb root access is enabled.

3. Preparing build environment:

    All the fuzzers in this change need to be built with "hwasan userdebug"
    configuration. For the blueline device, that will be the
    "blueline_hwasan-userdebug" option from lunch menu. All the commands from
    the rest steps need to run from this environment.

4. `fuzz.sh` helper script:

    There are quite a lot setups required to build and run the fuzzer. To make
    it easier, a `fuzz.sh` helper script is included. It needs to be run with
    current working directory set to the fuzzer directory. You can also source
    it and then use the "fuzz" function defined in it.

5. Building the fuzzer:

    Here we use "nci" fuzzer as an example for building and running. The same
    steps apply to the other two fuzzers.

    Run the following commands to build the fuzzer:

        cd $ANDROID_BUILD_TOP/system/nfc/src/fuzzers/nci
        ../fuzz.sh build

    The above commands will build the nci fuzzer and push it to the device.

6. Running the fuzzer:

    To run the fuzzer for a single iteration, assuming you are at the fuzzer
    directory ($ANDROID_BUILD_TOP/system/nfc/src/fuzzers/nci), all you need is
    this command:

          ../fuzz.sh run --once

    This will run the fuzzer and stop when a crash is found. The crash log will
    be printed on console directly.

    To run the fuzzer repeatedly, simply use the same command but without
    "--once" option. It will keep the fuzzer running, and do the following
    processfor every time a crash is found:

    1. Collecting the binary input causing the crash
    2. Re-running the fuzzer with this input to verify the crash reproduces
    3. Parsing the crash log to find out the crashing source location
    4. Archiving all the logs into a directory named by the crashing function
    5. Collecting code goverage data

    By default "fuzz run" will continue with whatever left last time. However,
    you can make it a fresh restart by passing a "-c" argument.
  
7. Other tools provided by `fuzz.sh`:

    Here are some extra options provided by `fuzz.sh` script:
    * "fuzz gcov" pulls the coverage data from device, and generate a
    visualization of accumulated code coverage.
    * "fuzz gdb" setup gdb instance with the fuzz input so you can debug a
    specific crash.

