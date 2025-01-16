#!/bin/sh

ninja \
    -C "$EXT_MOUNT/build" \
    everest-core_gcovr_coverage
retValHTML=$?

# Copy the generated coverage report to the mounted directory in any case
cp -R "$EXT_MOUNT/build/everest-core_gcovr_coverage" "$EXT_MOUNT/gcovr-coverage"
touch "$EXT_MOUNT/gcovr-coverage-xml.xml"

if [ $retValHTML -ne 0 ]; then
    echo "Coverage HTML report failed with return code $retValHTML"
    exit $retValHTML
fi
