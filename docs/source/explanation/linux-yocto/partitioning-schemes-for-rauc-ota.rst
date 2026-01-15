.. _partitioning-schemes-for-rauc-ota:

#################################
Partitioning schemes for RAUC OTA
#################################

As there are many ways to set up partitions on the storage device for
RAUC-based updates, this chapter will only provide a few ideas. Your
actual implementation may be different in the end.

As a target, we would like to have:

-  two full-size A/B rootfs partitions
-  two full-size A/B boot partitions for bootloader and FIT image
   (containing kernel/initrd/device tree for secure boot)
-  one/two user data partition(s)
-  one small factory data partition that contains (read only) files that
   are programmed once during production and will never change
   (e.g. serial numbers, certification region config etc)

The user data partition can be mounted as overlayfs on specific folders
to store run time-generated data (e.g. log files, user configuration
files, certificates, ...).

A factory reset should be implemented that clears the overlayfs.

The most simple version of this is to use a single user data partition
and mount it as overlay (e.g. on */var*) both for slot A and B. Then all
changes in the overlay will survive an update of the underlying rootfs.
For an example on how to do this, refer to the BelayBox Yocto sources.
The Raspberry Pi uses three boot partitions, for most other boards only
two are needed.

.. code-block:: shell

   part --source bootimg-partition --ondisk mmcblk0 --fstype=vfat --label boot --active --align 4096 --fixed-size 512
   part --source bootimg-partition --ondisk mmcblk0 --fstype=ext4 --label boot_a --align 4096 --fixed-size 512
   part --source bootimg-partition --ondisk mmcblk0 --fstype=ext4 --label boot_b --align 4096 --fixed-size 512
   part --source rootfs --ondisk mmcblk0 --fstype=ext4 --label root_A --align 4096 --fixed-size 3000
   part --source rootfs --ondisk mmcblk0 --fstype=ext4 --label root_B --align 4096 --fixed-size 3000
   part --ondisk mmcblk0 --fstype=ext4 --label factory_data --align 4096 --fixed-size 128
   part --ondisk mmcblk0 --fstype=ext4 --label overlay --align 4096 --fixed-size 7000

The disadvantage of this is the following: If the configuration file
format changes due to an update of the underlying rootfs, a separate job
may need to be run on the first boot into the new slot to transfer the
configuration files to the new format. If the boot into the new slot
fails, it will fall back to the old slot. The older version then is
maybe not compatible with the new config file format, so a full fallback
is not possible in this case.

To allow for better config file migration with fallback, consider to use
two user data partitions and a separate migration task (e.g. in initrd)
that transfers the files from the old user data partition to the new
one. It may adapt the file format in the process. In this case, falling
back to the old rootfs will work as it will also use the older overlay
user partition.

If you have an eMMC device, consider using the hardware boot partition
feature that eMMC offers for the bootloader. This will enable atomic
switching between the active boot slots.
