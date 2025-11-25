.. _how_to_renesas_mpu_patch:

#####################################
Patch for Renesas MPU (RZ/G2L family)
#####################################

Save this content here as a file with the extension ``.patch``. Then apply it to the
``kas/yocto/kirkstone.yml`` file in the ``rz-community-bsp`` repository:

.. code-block:: bash

  From 0af5946f55b746a6e436c45249f559866fcaa848 Mon Sep 17 00:00:00 2001
  From: sach1n1 <sachin.s.dominic@gmail.com>
  Date: Wed, 28 Aug 2024 12:44:51 +0200
  Subject: [PATCH] Signed-off-by: <sachin.dominic.zn@renesas.com>

  Changes for everest.
  ---
   kas/yocto/kirkstone.yml | 24 ++++++++++++++++++++++++
   1 file changed, 24 insertions(+)

  diff --git a/kas/yocto/kirkstone.yml b/kas/yocto/kirkstone.yml
  index b2a1470..c80dd34 100644
  --- a/kas/yocto/kirkstone.yml
  +++ b/kas/yocto/kirkstone.yml
  @@ -24,3 +24,27 @@ repos:
       layers:
         meta-arm-toolchain:
         meta-arm:
  +  meta-openembedded:
  +    url: https://github.com/openembedded/meta-openembedded
  +    commit: 52ecd66835dcfd8b4e55c9cb6325908ccea6a4e7
  +    layers:
  +      meta-oe:
  +      meta-networking:
  +      meta-python:
  +      meta-multimedia:
  +      meta-filesystems:
  +      meta-perl:
  +  meta-everest:
  +    url: https://github.com/EVerest/meta-everest.git
  +    commit: f9273939088db91a5699c07e512ddd7981e5637a
  +
  +
  +local_conf_header:
  +  systemd: |
  +    DISTRO_FEATURES:append = " systemd"
  +    VIRTUAL-RUNTIME_init_manager = "systemd"
  +    VIRTUAL-RUNTIME_initscripts = "systemd-compat-units"
  +
  +  everest-core: |
  +    IMAGE_INSTALL:append = " systemd systemd-analyze everest-core mosquitto"
  +    IMAGE_INSTALL:remove = "busybox-syslog"
  -- 
  2.25.1
