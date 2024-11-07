#!/bin/bash

usage() {
    echo "Usage: $0 <everest-core-directory>"
    exit 1
}

if [ $# -ne 1 ] ; then
    usage
else
    EVEREST_CERTS_PATH="$1"/build/dist/etc/everest/certs
    rm -rf $EVEREST_CERTS_PATH
    mkdir $EVEREST_CERTS_PATH

    cp -r certs/ca $EVEREST_CERTS_PATH
    cp -r certs/client $EVEREST_CERTS_PATH
fi
