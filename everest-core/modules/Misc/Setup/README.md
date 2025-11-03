# Setup module API documentation
This module is responsible for setup tasks that might need privileged access, for example wifi configuration.

If not run as root user, set at least the following capabilities in your EVerest config file: CAP_NET_ADMIN, CAP_NET_RAW, CAP_DAC_OVERRIDE.
They will be passed on to the child processes such as wpa_cli etc.

## Periodically published variables
### everest_api/setup/var/supported_setup_features
This variable is published periodically and contains a JSON object with the supported features in the following form:
```json
{
    "localization": true,
    "setup_simulation": true,
    "setup_wifi": true
}
```

### everest_api/setup/var/hostname
This variable is published periodically and contains the hostname.

## Commands and variables published in response
### everest_api/setup/cmd/scan_wifi
If any arbitrary payload is published to this topic a list of available wifi networks is published on the following topic:
__everest_api/setup/var/wifi_info__

with the following payload format:
```json
[
    {
        "bssid": "00:11:22:33:44:55",
        "flags": [
            "WPA2-PSK-CCMP",
            "ESS"
        ],
        "frequency": 2437,
        "signal_level": -41,
        "ssid": "Example"
    },
    {
        "bssid": "66:77:88:99:aa:bb",
        "flags": [
            "WPA2-PSK-CCMP",
            "ESS"
        ],
        "frequency": 5180,
        "signal_level": -56,
        "ssid": "Example2"
    }
]
```
additionally general network device information is published on the following topic:
__everest_api/setup/var/network_device_info__

with the following payload format:
```json
[
    {
        "blocked": false,
        "interface": "wlan0",
        "ipv4": ["192.0.2.23"],
        "ipv6": ["2001:db8:0:0:0:0:0:23"],
        "mac": "00:11:22:33:44:55",
        "rfkill_id": "0",
        "wireless": true
    },
    {
        "blocked": false,
        "interface": "eth0",
        "ipv4": "192.0.2.42",
        "ipv6": ["2001:db8:0:0:0:0:0:42"],
        "mac": "11:22:33:44:55:66",
        "rfkill_id": "",
        "wireless": false
    }
]
```

additionally the list of configured wifi networks is published to the following topic:
__everest_api/setup/var/configured_networks__

with the following payload format:
```json
[
    {
        "connected": true,
        "interface": "wlan0",
        "network_id": 0,
        "signal_level": -56,
        "ssid": "Example"
    },
    {
        "connected": false,
        "interface": "wlan0",
        "network_id": 1,
        "signal_level": -100,
        "ssid": "Example2"
    }
]
```

### everest_api/setup/cmd/enable_wifi_scanning
If any arbitrary payload is published to this topic the list of available wifi networks and general network device information just mentioned is published periodically.

### everest_api/setup/cmd/disable_wifi_scanning
If any arbitrary payload is published to this topic the list of available wifi networks and general network device information stops being periodically published.

### everest_api/setup/cmd/rfkill_unblock
If a rfkill_id is published to this topic the wifi interface with this id will be unblocked.

### everest_api/setup/cmd/rfkill_block
If a rfkill_id is published to this topic the wifi interface with this id will be blocked.

### everest_api/setup/cmd/list_configured_networks
If any arbitrary payload is published to this topic the list of configured wifi networks is published to the following topic:
__everest_api/setup/var/configured_networks__

with the following payload format:
```json
[
    {
        "connected": true,
        "interface": "wlan0",
        "network_id": 0,
        "ssid": "Example"
    },
    {
        "connected": false,
        "interface": "wlan0",
        "network_id": 1,
        "ssid": "Example2"
    }
]
```

### everest_api/setup/cmd/add_network
To add a wifi network a payload with the following format must be published to this topic:
```json
{
    "interface": "wlan0",
    "ssid": "Example",
    "psk": "20fcb529dee0aad11b0568f553942850d06e4c4531c0d75b35345d580b300f78"
}
```
The PSK field can represent the passphrase instead using escaped quotes:
```json
{
    "interface": "wlan0",
    "ssid": "Example",
    "psk": "\"A_valid_passphrase\""
}
```
For open WiFi networks the psk must be an empty string `"psk": ""`.

For hidden networks an optional item is needed:
```json
{
    "interface": "wlan0",
    "ssid": "Example",
    "psk": "\"A_valid_passphrase\"",
    "hidden": true
}
```
When `hidden` is not supplied then it is assumed to be false.

### everest_api/setup/cmd/enable_network
To enable a wifi network a payload with the following format must be published to this topic:
```json
{
    "interface": "wlan0",
    "network_id": 0
}
```

### everest_api/setup/cmd/disable_network
To disable a wifi network a payload with the following format must be published to this topic:
```json
{
    "interface": "wlan0",
    "network_id": 0
}
```

### everest_api/setup/cmd/select_network
To select a wifi network a payload with the following format must be published to this topic:
```json
{
    "interface": "wlan0",
    "network_id": 0
}
```

### everest_api/setup/cmd/remove_network
To remove a wifi network a payload with the following format must be published to this topic:
```json
{
    "interface": "wlan0",
    "network_id": 0
}
```

### everest_api/setup/cmd/remove_all_networks
If any arbitrary payload is published to this topic all wifi networks will be removed.

### everest_api/setup/cmd/enable_ap
If any arbitrary payload is published to this topic a wireless access point will be enabled on the interface configured in the module config.

### everest_api/setup/cmd/disable_ap
If any arbitrary payload is published to this topic the wireless access point will be disabled.

### everest_api/setup/cmd/check_online_status
If any arbitrary payload is published to this topic a ping will be sent to the host configured in the configuration key "online_check_host". Depending on the success of this ping a status of "online" or "offline" will be reported on the following topic:
__everest_api/setup/var/online_status__

### everest_api/setup/cmd/reboot
If any arbitrary payload is published to this topic the system will reboot.

## Application Info / Localization
### everest_api/setup/cmd/set_mode
If a mode _private_ or _public_ is published to this topic it will be stored permanently.

### everest_api/setup/cmd/set_initialized
If any arbitrary payload is published to this topic the system will be marked as "initialized" permanently.

### everest_api/setup/cmd/reset_initialized
If any arbitrary payload is published to this topic the system will be marked as "uninitialized" permanently.

### everest_api/setup/cmd/change_default_language
You can set a [three-letter language code](https://en.wikipedia.org/wiki/List_of_ISO_639-2_codes) to be set as the default language which will be stored permanently.

### everest_api/setup/cmd/change_current_language
You can set a [three-letter language code](https://en.wikipedia.org/wiki/List_of_ISO_639-2_codes) to be set as the current language.

### everest_api/setup/cmd/get_application_info
If any arbitrary payload is published to this topic a application info object is published to the following topic:
__everest_api/setup/var/application_info__

with the following payload format:
```json
{
    "initialized": true,
    "mode": "private",
    "default_language": "eng",
    "current_language": "ger"
}
```
