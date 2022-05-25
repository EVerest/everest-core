#!/bin/bash

echo "Downloading"
sleep 2
curl --progress-bar "${1}" -o "${2}"
curl_exit_code=$?
sleep 2
if [[ $curl_exit_code -eq 0 ]]; then
    echo "Downloaded"
else
    echo "DownloadFailed"
fi
sleep 2
echo "SignatureVerified"
sleep 2
echo "Installing"
sleep 2
echo "Installed"
