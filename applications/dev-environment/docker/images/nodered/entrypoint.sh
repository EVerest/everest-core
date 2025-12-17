#!/bin/sh

exec npm \
    --no-update-notifier \
    --no-fund \
    start \
    --cache /data/.npm \
    -- \
    --userDir /data \
    "$@"
