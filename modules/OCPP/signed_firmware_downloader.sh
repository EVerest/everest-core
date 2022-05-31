#!/bin/bash

mkdir /tmp/signature_validation
sleep 2
echo "Downloading"

sleep 2
curl --progress-bar "${1}" -o "${2}"
curl_exit_code=$?
sleep 2
if [[ $curl_exit_code -eq 0 ]]; then
    echo "Downloaded"
    echo -e "${3}" >/tmp/signature_validation/firmware_signature.base64
    echo -e "${4}" >/tmp/signature_validation/firmware_cert.pem
    openssl x509 -pubkey -noout -in /tmp/signature_validation/firmware_cert.pem >/tmp/signature_validation/pubkey.pem
    openssl base64 -d -in /tmp/signature_validation/firmware_signature.base64 -out /tmp/signature_validation/firmware_signature.sha256
    r=$(openssl dgst -sha256 -verify /tmp/signature_validation/pubkey.pem -signature /tmp/signature_validation/firmware_signature.sha256 "${2}")

    if [ "$r" = "Verified OK" ]; then
        echo "SignatureVerified"
    else
        echo "InvalidSignature"
    fi
else
    echo "DownloadFailed"
fi

rm -rf /tmp/signature_validation
