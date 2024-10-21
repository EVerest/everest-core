#!/bin/sh

# Run coverage HTML
ninja -C "$EXT_MOUNT/build" everest-core_gcovr_coverage_html
retVal=$?

cp -R "$EXT_MOUNT/build/everest-core_gcovr_coverage_html" "$EXT_MOUNT/gcovr-coverage"

if [ $retVal -ne 0 ]; then
    echo "Coverage HTML report failed with return code $retVal"
    exit $retVal
fi

# Run coverage XML
ninja -C "$EXT_MOUNT/build" everest-core_gcovr_coverage_xml
retVal=$?

cp "$EXT_MOUNT/build/everest-core_gcovr_coverage_xml.xml" "$EXT_MOUNT/gcovr-coverage-xml.xml"

if [ $retVal -ne 0 ]; then
    echo "Coverage XML report failed with return code $retVal"
    exit $retVal
fi
