#!/bin/bash

. "${1}"

echo "$UPLOADING"
sleep 2
curl --progress-bar --connect-timeout "$CONNECTION_TIMEOUT" -T "${4}" "${2}"
curl_exit_code=$?
if [[ $curl_exit_code -eq 0 ]]; then
    echo "$UPLOADED"
elif [[ $curl_exit_code -eq 67 ]] || [[ $curl_exit_code -eq 35 ]] || [[ $curl_exit_code -eq 69 ]] ||
    [[ $curl_exit_code -eq 9 ]]; then
    echo "$PERMISSION_DENIED"
elif [[ $curl_exit_code -eq 3 ]] || [[ $curl_exit_code -eq 6 ]] || [[ $curl_exit_code -eq 10 ]] ||
    [[ $curl_exit_code -eq 87 ]]; then
    echo "$BAD_MESSAGE"
elif [[ $curl_exit_code -eq 1 ]]; then
    echo "$NOT_SUPPORTED_OPERATION"
else
    echo "$UPLOAD_FAILURE"
fi
