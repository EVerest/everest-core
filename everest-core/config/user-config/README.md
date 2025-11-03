# User provided configuration
You can add user provided configuration files in this folder that can augment
a loaded config of the same name.
For example, if you want to change the ***evse_id*** of the
***connector_1*** in the [config-sil.yaml](../config-sil.yaml) to a value
specific to your own naming you can achieve this by adding a
*config-sil.yaml* file in this user-config folder with the following content:

```yaml
connector_1:
  config_module:
    evse_id: "DE*YOURCOMPANY*E12345*1"
```

This *user-config* does not have to be a *valid* configuration file; it just
needs to include the keys you want to set/overwrite in the respective config.
