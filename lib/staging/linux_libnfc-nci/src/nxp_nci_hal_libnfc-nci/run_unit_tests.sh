#!/bin/sh

known_tests=(
  nfc_test_utils
)

known_remote_tests=(
)


usage() {
  binary="$(basename "$0")"
  echo "Usage: ${binary} --help"
  echo "       ${binary} [-i <iterations>] [-s <specific device>] [--all] [<test name>[.<filter>] ...] [--<arg> ...]"
  echo
  echo "Unknown long arguments are passed to the test."
  echo
  echo "Known test names:"

  for name in "${known_tests[@]}"
  do
    echo "    ${name}"
  done

  echo
  echo "Known tests that need a remote device:"
  for name in "${known_remote_tests[@]}"
  do
    echo "    ${name}"
  done
}

iterations=1
device=
tests=()
test_args=()
while [ $# -gt 0 ]
do
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    -i)
      shift
      if [ $# -eq 0 ]; then
        echo "error: number of iterations expected" 1>&2
        usage
        exit 2
      fi
      iterations=$(( $1 ))
      shift
      ;;
    -s)
      shift
      if [ $# -eq 0 ]; then
        echo "error: no device specified" 1>&2
        usage
        exit 2
      fi
      device="$1"
      shift
      ;;
    --all)
      tests+=( "${known_tests[@]}" )
      shift
      ;;
    --*)
      test_args+=( "$1" )
      shift
      ;;
    *)
      tests+=( "$1" )
      shift
      ;;
  esac
done

if [ "${#tests[@]}" -eq 0 ]; then
  tests+=( "${known_tests[@]}" )
fi

adb=( "adb" )
if [ -n "${device}" ]; then
  adb+=( "-s" "${device}" )
fi

failed_tests=()
for spec in "${tests[@]}"
do
  name="${spec%%.*}"
  binary="/data/nativetest/${name}/${name}"

  push_command=( "${adb[@]}" push {"${ANDROID_PRODUCT_OUT}",}"${binary}" )
  test_command=( "${adb[@]}" shell "${binary}" )
  if [ "${name}" != "${spec}" ]; then
    filter="${spec#*.}"
    test_command+=( "--gtest_filter=${filter}" )
  fi
  test_command+=( "${test_args[@]}" )

  echo "--- ${name} ---"
  echo "pushing..."
  "${push_command[@]}"
  echo "running..."
  failed_count=0
  for i in $(seq 1 ${iterations})
  do
    "${test_command[@]}" || failed_count=$(( $failed_count + 1 ))
  done

  if [ $failed_count != 0 ]; then
    failed_tests+=( "${name} ${failed_count}/${iterations}" )
  fi
done

if [ "${#failed_tests[@]}" -ne 0 ]; then
  for failed_test in "${failed_tests[@]}"
  do
    echo "!!! FAILED TEST: ${failed_test} !!!"
  done
  exit 1
fi

exit 0
