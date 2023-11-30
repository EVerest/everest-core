# User provided configuration
You can add user provided configuration files in this folder that can augment
a loaded config of the same name.
For example, if you want to change the ***serial_port*** of the
***yeti_driver*** in the [config-hil.yaml](../config-hil.yaml) to a more
appropriate value on your machine you can achieve this by adding a
*config-hil.yaml* file in this user-config folder with the following content:

```yaml
yeti_driver:
  config_module:
    serial_port: "/dev/ttyUSB0"
```

This *user-config* does not have to be a *valid* configuration file, it just
needs to include the keys you want to set/overwrite in the respective config.
