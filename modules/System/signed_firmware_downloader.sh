#!/bin/bash

. "${1}"

SIGNATURE_VALIDATION_DIR=$(mktemp -d /tmp/signature_validation_XXXXX)
sleep 2
echo "$DOWNLOADING"

sleep 2
curl --progress-bar --connect-timeout "$CONNECTION_TIMEOUT" "${2}" -o "${3}"
curl_exit_code=$?
sleep 2
if [[ $curl_exit_code -eq 0 ]]; then
    echo "$DOWNLOADED"
    echo -e "${4}" >"$SIGNATURE_VALIDATION_DIR/firmware_signature.base64"
    echo -e "${5}" >"$SIGNATURE_VALIDATION_DIR/firmware_cert.pem"
    openssl x509 -pubkey -noout -in "$SIGNATURE_VALIDATION_DIR/firmware_cert.pem" >"$SIGNATURE_VALIDATION_DIR/pubkey.pem"
    openssl base64 -d -in "$SIGNATURE_VALIDATION_DIR/firmware_signature.base64" -out "$SIGNATURE_VALIDATION_DIR/firmware_signature.sha256"
    r=$(openssl dgst -sha256 -verify "$SIGNATURE_VALIDATION_DIR/pubkey.pem" -signature "$SIGNATURE_VALIDATION_DIR/firmware_signature.sha256" "${3}")

    if [ "$r" = "Verified OK" ]; then
        echo "$SIGNATURE_VERIFIED"
    else
        echo "$INVALID_SIGNATURE"
    fi
else
    echo "$DOWNLOAD_FAILED"
fi

rm -rf "$SIGNATURE_VALIDATION_DIR"
