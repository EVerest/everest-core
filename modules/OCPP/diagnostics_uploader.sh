#!/bin/bash

echo "uploading"
curl --progress-bar -T "${3}" "${1}"
curl_exit_code=$?
if [[ $curl_exit_code -eq 0 ]]
then
    echo "uploaded"
else
    echo "upload_failed"
fi
