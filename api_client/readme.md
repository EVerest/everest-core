# What is the EVerest API client utility
The API client utility is a command line tool written in Python that can be used to communicate over EVerest API or to spy/debug the communication. You can use the tool as well to simulate or test certain interface implementations, if you are in the development phase.
The tool is generated out of the EVerest Async API interface definitions (see doc/everest_api_specs) and can be installed on the productive systems or used for development purposes.
Similar to a terminal, the tool supports help, command completion, arguments completion, command history, formatting options, scripting, macros, aliases, etc.

## Modular architecture
Communicating with a real system all at once over EVerest API would make the output hard to follow, thus the tool allows you to concentrate only on the interface you want to look at.
You can however spy the entire communication.
For each EVerest API interface, there is a API client module that you can create.

## EVerest API interfaces
The following modules are available, each corresponding to a EVerest API interface.
* `AuthConsumerAPIClient`
* `AuthTokenProviderAPIClient`
* `AuthTokenValidatorAPIClient`
* `DisplayMessageAPIClient`
* `ErrorHistoryConsumerAPIClient`
* `EvseBspAPIClient`
* `EvseManagerConsumerAPIClient`
* `IsolationMonitorAPIClient`
* `OcppConsumerAPIClient`
* `PowermeterAPIClient`
* `PowerSupplyDCAPIClient`
* `SystemAPIClient`

An additional module has been created to spy on the communication of a module or for the entire system: `SpyClient`

# Installation and requirements
The API client is part of the EVerest API, however it is not build by default.
In order to build and install the tool you need the following `cmake` flags:
* EVEREST_ENABLE_PY_SUPPORT
* EVEREST_BUILD_API_CLIENT

The tool is written in Python so, obviously in order to run it you would require Python3 and the following modules:
* paho-mqtt
* uuid
* os
* json
* argparse
* cmd2
* queue
* threading

In addition to that the tool requires the modules WHL files to be installed on the target system (check the [Build](#build) chapter).

Since the tool is generated from the Async API Async specification the Async API cli tools need to be installed. We are installing the tools automatically during `cmake` phase if `node` and `npm` are already available.

## Build
In order to build EVerest with the API client you need to pass the following 2 options `-DEVEREST_ENABLE_PY_SUPPORT=On` and `-DEVEREST_BUILD_API_CLIENT=On` to `cmake`.

By default the build and install process will create the modules listed above as WHL package.
If nothing has been changed, they are in the `build\dist-wheels` folder and can be installed (use python environment if necessary):

`pip install build/dist-wheels/* --force-reinstall`

For Yocto, make sure to add the same options in the EVerest recipe.

# Usage

You can use the API tool directly on a productive system, if all the Python modules have been installed locally.

Alternatively, you can forward the MQTT communication on your machine and run the tool on local machine (MQTT supports natively bridging the communication from one broker to another). Alternatively you can use `ssh` to forward the traffic.

## Starting the tool
Once that the dependencies are installed, you can run the tool:
`bc_api_client.py` (normally found under `/bin` on the target system or in the `build/dist/bin` folder for the build system).
Once the tool loads, you can type help to see the list of supported modules and their commands:
```
(Cmd) help

Documented commands (use 'help -v' for verbose/'help <topic>' for details):

AuthConsumerAPIClient
=====================
tc_receive_token_validation_status

AuthTokenProviderAPIClient
==========================
tp_send_provided_token

AuthTokenValidatorAPIClient
===========================
tv_receive_request_validate_token  tv_send_reply_validate_token

Client Management
=================
client_add  client_remove  receive_heartbeat  send_communication_check

DisplayMessageAPIClient
=======================
dm_receive_request_clear_display_message  dm_send_reply_clear_display_message
dm_receive_request_get_display_message    dm_send_reply_get_display_message
dm_receive_request_set_display_message    dm_send_reply_set_display_message

ErrorHistoryConsumerAPIClient
=============================
eh_receive_error_cleared  eh_receive_reply_active_errors
eh_receive_error_raised   eh_send_request_active_errors

EvseBspAPIClient
================
bsp_receive_ac_overcurrent_limit                   bsp_send_reply_reset
bsp_receive_ac_switch_three_phases_while_charging
bsp_receive_allow_power_on
bsp_receive_enable
bsp_receive_evse_replug
bsp_receive_lock
bsp_receive_pwm_F
bsp_receive_pwm_off
bsp_receive_pwm_on
bsp_receive_request_ac_pp_ampacity
bsp_receive_request_reset
bsp_receive_self_test
bsp_receive_unlock
bsp_send_ac_nr_of_phases
bsp_send_ac_pp_ampacity
bsp_send_capabilities
bsp_send_clear_error
bsp_send_event
bsp_send_raise_error
bsp_send_rcd_current
bsp_send_reply_ac_pp_ampacity

EvseManagerConsumerAPIClient
============================
em_receive_ac_nr_of_phases_available
em_receive_ac_pp_ampacity
em_receive_car_manufacturer
em_receive_dc_capabilities
em_receive_dc_mode
em_receive_dc_voltage_current
em_receive_dlink_ready
em_receive_ev_info
em_receive_evse_id
em_receive_hw_capabilities
em_receive_isolation_measurement
em_receive_limits
em_receive_powermeter
em_receive_random_delay_countdown
em_receive_ready
em_receive_reply_enable_disable
em_receive_reply_external_ready_to_start_charging
em_receive_reply_force_unlock
em_receive_reply_get_evse
em_receive_reply_pause_charging
em_receive_reply_reserve
em_receive_reply_resume_charging
em_receive_reply_stop_transaction
em_receive_request_iso15118_certificate
em_receive_selected_protocol
em_receive_session_event
em_receive_waiting_for_external_ready
em_send_authorize_response
em_send_cancel_reservation
em_send_random_delay_cancel
em_send_random_delay_disable
em_send_random_delay_enable
em_send_random_delay_set_duration_s
em_send_reply_iso15118_certificate
em_send_request_enable_disable
em_send_request_external_ready_to_start_charging
em_send_request_force_unlock
em_send_request_get_evse
em_send_request_pause_charging
em_send_request_reserve
em_send_request_resume_charging
em_send_request_stop_transaction
em_send_withdraw_authorization

IsolationMonitorAPIClient
=========================
im_receive_start            im_send_isolation_measurement
im_receive_start_self_test  im_send_raise_error
im_receive_stop             im_send_self_test_result
im_send_clear_error

OcppConsumerAPIClient
=====================
oc_receive_boot_notification_response      oc_send_request_get_variables
oc_receive_is_connected                    oc_send_request_set_variables
oc_receive_reply_data_transfer_outgoing
oc_receive_reply_get_variables
oc_receive_reply_set_variables
oc_receive_request_data_transfer_incoming
oc_receive_security_event
oc_send_reply_data_transfer_incoming
oc_send_request_data_transfer_outgoing

PowermeterAPIClient
===================
pm_receive_request_start_transaction  pm_send_public_key_ocmf
pm_receive_request_stop_transaction   pm_send_reply_start_transaction
pm_send_powermeter_values             pm_send_reply_stop_transaction

PowerSupplyDCAPIClient
======================
ps_receive_export_voltage_current  ps_send_clear_error
ps_receive_import_voltage_current  ps_send_mode
ps_receive_mode                    ps_send_raise_error
ps_send_capabilities               ps_send_voltage_current

SystemAPIClient
===============
sys_receive_allow_firmware_installation  sys_send_firmware_update_status
sys_receive_request_get_boot_reason      sys_send_log_status
sys_receive_request_is_reset_allowed     sys_send_reply_get_boot_reason
sys_receive_request_set_system_time      sys_send_reply_is_reset_allowed
sys_receive_request_update_firmware      sys_send_reply_set_system_time
sys_receive_request_upload_logs          sys_send_reply_update_firmware
sys_receive_reset                        sys_send_reply_upload_logs

Uncategorized
=============
alias  help     macro  quit          run_script  shell
edit   history  py     run_pyscript  set         shortcuts
```
You can get additional help by typing `help` followed by the command you are interested.

```
(Cmd) help client_add
Usage: client_add [-h] {SpyClient, PowerSupplyDCAPIClient, AuthTokenValidatorAPIClient, SystemAPIClient, ErrorHistoryConsumerAPIClient, AuthConsumerAPIClient, AuthTokenProviderAPIClient, EvseBspAPIClient, IsolationMonitorAPIClient, OcppConsumerAPIClient, EvseManagerConsumerAPIClient, DisplayMessageAPIClient, PowermeterAPIClient} ...

Creates a client with the given module id

optional arguments:
  -h, --help            show this help message and exit

client_type:
  {SpyClient, PowerSupplyDCAPIClient, AuthTokenValidatorAPIClient, SystemAPIClient, ErrorHistoryConsumerAPIClient, AuthConsumerAPIClient, AuthTokenProviderAPIClient, EvseBspAPIClient, IsolationMonitorAPIClient, OcppConsumerAPIClient, EvseManagerConsumerAPIClient, DisplayMessageAPIClient, PowermeterAPIClient}
                        the client type to be added
```

Certain commands have variable arguments, you can query them by providing partial arguments:

```
(Cmd) help client_add SpyClient
Usage: client_add SpyClient [-h] [mqtt_topic]

positional arguments:
  mqtt_topic   the path/topic for the client to spy on, if empty will spy on everest

optional arguments:
  -h, --help  show this help message and exit
```

## Spying the EVerest API

The easiest usage of the client tool is to spy on the communication.
To achieve this, one has to add the SpyClient and optionally provide it with the topic it should spy on.
Outgoing messages will be displayed in yellow (display prefix as `Out:`), incoming in green (display prefix `In:`):

```
(Cmd) client_add SpyClient
```
or
```
(Cmd) client_add SpyClient everest/api/1.0/display_message/dm_1/e2m/set_display_message
In: everest/api/1.0/display_message/dm_1/e2m/set_display_message, {"headers":{"replyTo":"everest/api/1.0/display_message/dm_1/m2e/reply/10dea131-dc90-4a66-adc1-9b85948223f5"},"payload":[{"id":0,"identifier_type":"TransactionId","message":{"content":"","format":"UTF8"}}]}
```

## Correspondence between the EVerest API modules in your EVerest config and API client modules

For each and every EVerest API interface, we have a corresponding api client module that can be added with the command `client_add`.
You need to provide as well the module id for each added client. The module id needs to be the one defined in the EVerest config file.

## Communicating over EVerest API

Let's take an example of communicating over EVerest API. We are going to use 3 interfaces that normally work together to authenticate a token.
We have:
* authentication token provider module in EVerest API - its role is to provide the token
* authentication token validator module in EVerest API - its role is to validate the token provided by the provider
* authentication token consumer module in EVerest API - its role is to observe the status of the token passed between the above interfaces

Let's assume that we have a EVerest config with the above modules. Here is an example how this could look like:
```
  auth_consumer_1:
    module: auth_consumer_API
    config_module:
      cfg_communication_check_to_s: 60
      cfg_heartbeat_interval_ms: 40000
    connections:
      auth:
        - implementation_id: main
          module_id: auth
  auth_token_provider_1:
    module: auth_token_provider_API
    config_module:
      cfg_communication_check_to_s: 60
      cfg_heartbeat_interval_ms: 40000
  auth_token_validator_1:
    module: auth_token_validator_API
    config_module:
      cfg_communication_check_to_s: 60
      cfg_heartbeat_interval_ms: 40000
```
The complete config can be found in the `config` folder as `config-auth-example.yaml`.
First thing we are going to do is to add a spy client to see our communication.
Type `client_add SpyClient` and press enter.
Very often in a real system, there is a lot of communication going on, you can turn of and on certain topics.
For example you can turn off topics that contain the substring `limits` and substring `powermeter` by typing: `set hide_topic "powermeter, limits"`.
Or I can turn off all topics by typing `set hide_topic everest` and then showing only certain topics e.g. `set show_topic limits` (you can add more by having them comma separated and the entire thing between simple or double quotes).

Let's create the corresponding client api modules to the ones from EVerest API. Each client is created with the corresponding module id as per configuration file above. The module id is used by every subsequent command in order to communicate with the right module in EVerest. Press TAB at any moment for command completion or argument completion.
```
(Cmd) client_add AuthTokenProviderAPIClient auth_token_provider_1
(Cmd) client_add AuthTokenValidatorAPIClient auth_token_validator_1
(Cmd) client_add AuthConsumerAPIClient auth_consumer_1
```
In order to initiate a token validation process type the following command (this will provide the token `TOKEN` to the system by the `auth_token_provider_1` module):
```
p_send_provided_token auth_token_provider_1 {"authorization_type":"OCPP","id_token":{"type":"Central","value":"TOKEN"}}
```
The system will send back the processing status and the request to validate the token:
```
In: everest/api/1.0/auth_consumer/auth_consumer_1/e2m/token_validation_status, {"status": "Processing", "token": {"authorization_type": "OCPP", "id_token": {"type": "Central", "value": "TOKEN"}}}
In: everest/api/1.0/auth_token_validator/auth_token_validator_1/e2m/validate_token, {"headers":{"replyTo":"everest/api/1.0/auth_token_validator/auth_token_validator_1/m2e/reply/ddb52aef-d64f-4668-abca-db0b27d92e19"},"payload":{"authorization_type":"OCPP","id_token":{"type":"Central","value":"TOKEN"}}}
```

In order to validate the token send the following command (you need to reuse the replyTo value from the request):
```
tv_send_reply_validate_token auth_token_validator_1 everest/api/1.0/auth_token_validator/auth_token_validator_1/m2e/reply/ddb52aef-d64f-4668-abca-db0b27d92e19 {"authorization_status":"Accepted","evse_ids":[1]}
```

This exemplifies a request/reply exchange over EVerest API. In this case you receive a request containing an address that you have to use to send your reply. The alternative is also possible, you send the request and you provide an address that the other side has to use to send back the response. Please note that by convention the `replyTo` params are using the following format of the address:
```
everest/api/1.0/{interface_type}/{module_id}/e2m/{operation_name}/{uuid}
```
API client has a pattern detection mechanism and it detects the above text, being able to generate the actual values. Adding a spy client would allow you to see the message being send and check the address.
In case you have the SpyClient loaded, all the incoming messages will be doubled since both the SpyClient and the respective module will receive and display the respective messages.
You could repeat the example without the spy client loaded in case things are looking too crowded.

In general, make sure to use `hide_topic` and `show_topic` to filter topics otherwise the output might be overwhelming. First the `hide_topic` is applied while `show_topic` will be punching holes.
This would allow you to disable all the traces setting `hide_topic` to `everest` (for example) and then punch holes for the topics you want to see.

The tool is saving the history containing the executed commands and by pressing the up/down arrows you can review, edit and execute them.
You can generate transcripts (a file that contains the command and regular expressions for the result of the command) from the history allowing you to use the tool as testing tool too.

## EVerest API settings

The api client has the following settings influencing the output:
```
(Cmd) set
Name                  Value                           Description
==================================================================================================================
allow_style           Terminal                        Allow ANSI text style sequences in output (valid values:
                                                      Always, Never, Terminal)
always_show_hint      False                           Display tab completion hint even when completion suggestions
                                                      print
colors                True                            Use colors for the outputs
debug                 False                           Show full traceback on exception
echo                  False                           Echo command issued into output
editor                vim                             Program used by 'edit'
feedback_to_output    False                           Include nonessentials in '|', '>' results
hide_topic                                            Comma separate list. If set hides ONLY the messages with
                                                      topics matching the list's strings. If show_topic is set
                                                      too, hide_topic is applied first, then show_topic.
max_completion_items  50                              Maximum number of CompletionItems to display during tab
                                                      completion
prettify              False                           Prettify the json outputs
quiet                 True                            Don't print nonessential feedback
replyToPlaceholder    everest/api/1.0/{interface_ty   The placeholder to search and replace in a payload
                      pe}/{module_id}/e2m/{operation
                      _name}/{uuid}
show_topic                                            Comma separated list. If set shows ONLY the messages with
                                                      topics matching the list's strings. If hide_topic is set
                                                      too, it will be applied first, then show_topic will be
                                                      applied.
timing                False                           Report execution times
```

## Other EVerest API client available features

The tool can be used to create tests or can be used to simulate certain parts of the EVerest API.

* *Aliases and macros* - the tool allows you to create aliases and macros for most used commands.
* *Shell commands* - The tool allows you to execute shell script commands (e.g. `shell ls` or `!ls`)
* *Transcript generation and execution* - If you have executed a set of commands and you want to create a test script for it check the command `history` with the option `-t`.
