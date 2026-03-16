#!/bin/sh
set -ex

echo "Starting the manager"

bin/manager --prefix . --config etc/everest/config.yaml &
PID_MANAGER=$!

sleep 5
echo "Exit"

if ps -p $PID_MANAGER > /dev/null
then
    kill $PID_MANAGER
else
    echo "manager died"
    exit -1
fi