# everest_release.json

A JSON file containing a subset of installed packages can be added to your
image `/etc/everest/everest_release.json` by adding the following line to
your image recipes

```
inherit everest_version_file
```

`everest_version_file.bbclass` queries the installed packages and creates the
release JSON file by appending `do_everest_generate_version` to
the `ROOTFS_POSTPROCESS_COMMAND` variable.

`everest_version_file.bbclass` contains a list of packages to include.

This list can be extended via the `EVEREST_RELEASE_PACKAGES` variable as a
space separated list. For example in `local.conf`

```
everest_RELEASE_PACKAGES += "systemd tcpdump"
```

Note that packages need to be listed in `RDEPENDS`, or `IMAGE_INSTALL` to be
available for inclusion.
