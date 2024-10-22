#!/bin/bash

# Due to the complexity to android buile environment this script is created for
# development purpose to build, run and debug the fuzzers. It's not needed or
# required for official build and fuzzing.

function init() {
  if [ -z "$ANDROID_BUILD_TOP" ];
  then
    echo "Did you forget lunch?"
    exit 1
  fi
  source $ANDROID_BUILD_TOP/build/envsetup.sh

  PROJ=$(basename $PWD)
  FUZZER_NAME=nfc_${PROJ}_fuzzer
  FUZZ_DIR=data/fuzz/$(get_build_var TARGET_ARCH)/$FUZZER_NAME
  FUZZ_OPTIONS="$FUZZ_OPTIONS -close_fd_mask=3 -max_len=512 -artifact_prefix=/$FUZZ_DIR/crashes/"
}

function run_once() {
  if [ "$1" == "-c" ];
  then
    adb shell rm -rf /$FUZZ_DIR/corpus /$FUZZ_DIR/crashes /$FUZZ_DIR/gcov
    adb shell mkdir -p /$FUZZ_DIR/corpus /$FUZZ_DIR/crashes /$FUZZ_DIR/gcov
    adb push ./corpus/* /$FUZZ_DIR/corpus/  >/dev/null 2>&1
    rm -rf ./logs ./coverage

    shift
  fi

  adb logcat -c
  if [ -z "$1" ];
  then
    PAYLOAD=/$FUZZ_DIR/corpus
    echo "Fuzzing with corpus from $PAYLOAD..."
  else
    PAYLOAD=$1
    echo "Verifying payload $PAYLOAD..."
  fi

  adb shell mkdir -p /$FUZZ_DIR/corpus /$FUZZ_DIR/crashes /$FUZZ_DIR/gcov
  adb shell LD_LIBRARY_PATH=/system/lib64/vndk-29 GCOV_PREFIX=/$FUZZ_DIR/gcov GCOV_PREFIX_STRIP=3 /$FUZZ_DIR/$FUZZER_NAME $FUZZ_OPTIONS $PAYLOAD

  echo "==========================================================================================="
  adb logcat -d| $ANDROID_BUILD_TOP/external/compiler-rt/lib/asan/scripts/symbolize.py
}

function run_fuzz() {
  if [ "$1" == "-c" ];
  then
    adb shell rm -rf /$FUZZ_DIR/corpus /$FUZZ_DIR/crashes /$FUZZ_DIR/gcov
    adb shell mkdir -p /$FUZZ_DIR/corpus /$FUZZ_DIR/crashes /$FUZZ_DIR/gcov
    adb push ./corpus/* /$FUZZ_DIR/corpus/  >/dev/null 2>&1
    rm -rf ./logs ./coverage
  fi

  mkdir -p ./logs/ERROR ./logs/UNKNOWN ./coverage
  while true
  do
    echo "Running ..."
    TS=`date +"%m-%d-%Y-%H-%M-%S"`
    run_once >./logs/fuzz.log 2>&1

    echo "Fuzzer crashed, looking for crash input ..."
    CRASH=$(grep -aoP "Test unit written to \K\S+" ./logs/fuzz.log)
    if [ -z "$CRASH" ];
    then
      echo "Error, crash not found!"
      mv ./logs/fuzz.log ./logs/ERROR/run_$TS.log
      continue
    fi

    echo "Verifying crash ..."
    run_once $CRASH >./logs/verify.log 2>&1
    SIG=$(grep -m 1 -aoP "#?? \S+ in \K\S+ system/nfc/src\S+:\S+" ./logs/verify.log)
    if [ -z "$SIG" ];
    then
      SIG='UNKNOWN'
      cat ./logs/verify.log>>./logs/fuzz.log
    else
      cp ./logs/verify.log ./logs/fuzz.log
    fi

    SIG_DIR=$(echo $SIG | tr " /:" '#@#')
    if [ ! -d "./logs/$SIG_DIR" ];
    then
      echo "New crash category found: $SIG"
      mkdir -p ./logs/$SIG_DIR
    else
      echo "Known crash: $SIG"
    fi

    mv ./logs/fuzz.log ./logs/$SIG_DIR/run_$TS.log
    adb pull $CRASH ./logs/$SIG_DIR/crash_$TS.bin >/dev/null 2>&1
    adb rm $CRASH >/dev/null 2>&1
  done
}

function build() {
  pushd $ANDROID_BUILD_TOP
  SANITIZE_HOST="address" \
    SANITIZE_TARGET="hwaddress fuzzer" \
    NATIVE_COVERAGE="true" \
    COVERAGE_PATHS="system/nfc/src" \
    make -j $FUZZER_NAME
  popd
  adb shell mkdir -p /$FUZZ_DIR
  adb push $OUT/symbols/$FUZZ_DIR/$FUZZER_NAME /$FUZZ_DIR/
}

function run() {
  if [ "$1" == "--once" ];
  then
    shift
    run_once $@
  else
    echo "fuzzing..."
    run_fuzz $@
  fi
}

function debug() {
  if [ -z "$1" ];
  then
    echo "Which payload?"
    exit
  fi

  FUZZ_PAYLOAD=$1

  adb forward tcp:5039 tcp:5039
  adb shell LD_LIBRARY_PATH=/system/lib64/vndk-29 gdbserver64 remote:5039 /$FUZZ_DIR/$FUZZER_NAME $FUZZ_OPTIONS $FUZZ_PAYLOAD 2>&1 >/dev/null&
  sleep 5
  $ANDROID_BUILD_TOP/prebuilts/gdb/linux-x86/bin/gdb --directory=$ANDROID_BUILD_TOP -ex "target remote:5039"
}

function get_cov() {
  mkdir -p ./coverage && adb pull /$FUZZ_DIR/gcov/0/out/soong ./coverage
  unzip -o $OUT/coverage/$FUZZ_DIR/$FUZZER_NAME.zip -d ./coverage
  lcov --directory ./coverage --base-directory $ANDROID_BUILD_TOP --gcov-tool $(pwd)/../llvm-gcov --capture -o ./coverage/cov.info
  TS=`date +"%m-%d-%Y-%H-%M-%S"`
  genhtml ./coverage/cov.info -o ./coverage/report_$TS
  xdg-open ./coverage/report_$TS/index.html
}

function fuzz() {
  init
  action=$1
  shift

  case "$action" in
    run)
      run $@
      ;;
    build)
      build $@
      ;;
    debug)
      debug $@
      ;;
    gcov)
      get_cov $@
      ;;
    *)
      echo "Usage: $0 {run|build|debug|gcov}"
      exit 1
  esac
}

if [ "$0" == "${BASH_SOURCE[0]}" ];
then
  fuzz $@
fi

