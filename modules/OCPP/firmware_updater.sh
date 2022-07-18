#!/bin/bash

echo "downloading"
curl --progress-bar --connect-timeout 20 "${1}" -o "${2}"
curl_exit_code=$?
sleep 2
if [[ $curl_exit_code -eq 0 ]]; then
    echo "downloaded"
else
    echo "download_failed"
fi
sleep 2
echo "installing"
sleep 2
echo "installed"
sleep 2
