#!/bin/bash

echo "initialize.sh script used to re-initialize EVerest OCPP 2.0.1 device model and starting everest"

python3 /opt/everest/ocpp201config/init_device_model_db.py --db /opt/everest/share/everest/modules/OCPP201/device_model_storage.db --config /opt/everest/config/ocpp-config.json --schemas /opt/everest/ocpp201config/component_schemas/ init insert

/opt/everest/bin/manager --conf /opt/everest/config/config.yaml
