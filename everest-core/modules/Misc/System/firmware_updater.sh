#!/bin/bash

. "${1}"

echo "$DOWNLOADING"
curl --progress-bar --ssl --connect-timeout "$CONNECTION_TIMEOUT" "${2}" -o "${3}"
curl_exit_code=$?
sleep 2
if [[ $curl_exit_code -eq 0 ]]; then
    echo "$DOWNLOADED"
else
    echo "$DOWNLOAD_FAILED"
fi
sleep 2

if [[ $curl_exit_code -eq 0 ]]; then
    echo "$INSTALLING"
    sleep 2
    echo "$INSTALLED"
    sleep 2
fi
