.. _everest_modules_handwritten_CarloGavazzi_DCT1:

*******************************************
CarloGavazzi_DCT1
*******************************************

The module ``CarloGavazzi_DCT1`` implements powermeter interface to read data from 
DCT1 device that is connected via Modbus RTU. 


Testing the module
==================
We use the MQTT Explorer to interact with the CarloGavazzi_DCT1 module.

``start_transaction`` command 
-----------------------------
Publish a command to the topic: ``everest/power_meter_1/meter/cmd``

``Request JSON:``

  .. code-block:: JSON
 
        {
            "data": {
                "args": {
                    "value": {
                        "evse_id": "evse_id",
                        "transaction_id": "2995e453-6ba4-4d0f-8030-bec4396d8a63",
                        "client_id": "client_id",
                        "tariff_id": 0,
                        "cable_id": 0,
                        "user_data": ""
                    }
                },
                "id": "00000000-0000-0000-0000-000000000042",
                "origin": "manual_test"
            },
            "name": "start_transaction",
            "type": "call"
        }
        
``Response JSON:``

  .. code-block:: JSON

        {
            "data": {
                "id": "00000000-0000-0000-0000-000000000042",
                "origin": "power_meter_1",
                "retval": {
                    "status": "OK"
                }
            },
            "name": "start_transaction",
            "type": "result"
        }

``stop_transaction`` command 
-----------------------------

Publish a command to the topic: ``everest/power_meter_1/meter/cmd``

``Request JSON:``

  .. code-block:: JSON

        {
            "data": {
                "args": {
                    "transaction_id": "2995e453-6ba4-4d0f-8030-bec4396d8a63"
                },
                "id": "00000000-0000-0000-0000-000000000042",
                "origin": "manual_test"
            },
            "name": "stop_transaction",
            "type": "call"
        }

``Response JSON:``

  .. code-block:: JSON

    {
        "data": {
            "id": "00000000-0000-0000-0000-000000000042",
            "origin": "power_meter_1",
            "retval": {
            "signed_meter_value": {
                "encoding_method": "",
                "public_key": "0464F9F9447A00672486A6D4625AA5FDB2E8D44B5705347316E7975BC8F9B29FAA11BBF44E8E1E82270267C52D1896AB240C7B4000B9BA2152DE5CCE822E3290A0B1376BFAFE4FB3956B1777EC9EE91EE0671A046BC3433F1409E44B229B5C71E9",
                "signed_meter_data": "AQABCAD/AAAAHgAAAAAAAAAAAAABAAIIAP8AAAAeAAAAAAAAAAAAAAEAAQcA/wAAABv//wAAAAABAAwHAP8AAAAj//8AAAAAAQALBwD/AAAAIf/9AAAAAAEAAAoC/wAAACb//QAAAABEQ1QxQTMwVjEwTFMzRUMAAAAAAEJZMDUyMDAwMTAwMkwARENUMUEzMFYxMExTM0VDAHUjlewVGaSa37FIO2S4nVls1wH34HXM/VhqjCmVe2Dy8k/GEaa9zuMj2HY9uPlDwQ0bmq3qlIHesNgBbvcIiP7PXx/fJYrIn1/kgh/sLrUN5YkKefVBqQIkBK7vXk8KOw==",
                "signing_method": "384 bit ECDSA SHA 384, using curve brainpoolP384r1"
            },
            "start_signed_meter_value": {
                "encoding_method": "",
                "public_key": "0464F9F9447A00672486A6D4625AA5FDB2E8D44B5705347316E7975BC8F9B29FAA11BBF44E8E1E82270267C52D1896AB240C7B4000B9BA2152DE5CCE822E3290A0B1376BFAFE4FB3956B1777EC9EE91EE0671A046BC3433F1409E44B229B5C71E9",
                "signed_meter_data": "AQABCAD/AAAAHgAAAAAAAAAAAAABAAIIAP8AAAAeAAAAAAAAAAAAAAEAAQcA/wAAABv//wAAAAABAAwHAP8AAAAj//8AAAAAAQALBwD/AAAAIf/9AAAAAAEAAAoC/wAAACb//QAAAABEQ1QxQTMwVjEwTFMzRUMAAAAAAEJZMDUyMDAwMTAwMkwARENUMUEzMFYxMExTM0VDACiWQqialjeOBZEW8AonMrpBmsBRnAGVsS/dya+GyhYCxq7JWfz8mywhJi9udAvJRHgspjtQYofqmM0y3c3eJSz3TFfC9TsQNOpBZ2BC+CZRJ+C2ewIsE9D1EIdJyAGG1A==",
                "signing_method": "384 bit ECDSA SHA 384, using curve brainpoolP384r1"
            },
            "status": "OK"
            }
        },
        "name": "stop_transaction",
        "type": "result"
    }

Publish powermeter variables
----------------------------
The module reads the following powermeter parameters and publishs them to the EVerest system:

* energy_Wh_import
* energy_Wh_export
* power_W
* voltage_V
* current_A

Publish the values ​​every second.
