#!/bin/bash

echo "Uploading"
sleep 2
curl --progress-bar -T "${3}" "${1}"
curl_exit_code=$?
if [[ $curl_exit_code -eq 0 ]]; then
    echo "Uploaded"
elif [[ $curl_exit_code -eq 67 ]] || [[ $curl_exit_code -eq 69 ]] || [[ $curl_exit_code -eq 9 ]]; then
    echo "PermissionDenied"
elif [[ $curl_exit_code -eq 1 ]] || [[ $curl_exit_code -eq 3 ]] || [[ $curl_exit_code -eq 6 ]]; then
    echo "BadMessage"
elif [[ $curl_exit_code -eq 7 ]]; then
    echo "NotSupportedOperation"
else
    echo "UploadFailure"
fi
