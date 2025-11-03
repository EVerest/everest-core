#!/bin/sh

ninja \
    -C "$EXT_MOUNT/build" \
    everest-evse_security_gcovr_coverage
retValHTML=$?

ninja \
    -C "$EXT_MOUNT/build" \
    everest-evse_security_gcovr_coverage_xml
retValXML=$?

# Copy the generated coverage report and xml to the mounted directory in any case
cp -R "$EXT_MOUNT/build/everest-evse_security_gcovr_coverage" "$EXT_MOUNT/gcovr-coverage"
cp "$EXT_MOUNT/build/everest-evse_security_gcovr_coverage_xml.xml" "$EXT_MOUNT/gcovr-coverage-xml.xml"

if [ $retValHTML -ne 0 ]; then
    echo "Coverage HTML report failed with return code $retValHTML"
    exit $retValHTML
fi

if [ $retValXML -ne 0 ]; then
    echo "Coverage XML report failed with return code $retValXML"
    exit $retValXML
fi
