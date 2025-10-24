.. _exp_linux_yocto_ota_updates:

##########################
Over-the-air-updates (OTA)
##########################

One of the most important (and often underestimated) features of a
charging station is the ability to remotely update the software when the
charger is installed at the customer. Updates are needed to deploy for
example:

-  General bug fixes
-  Fixing compatibility issues with new EVs (or old EVs with new
   firmware versions)
-  Fixing compatibility issues with OCPP backends (or new versions
   deployed on the backend side)
-  Security issues
-  New features

Updates are often referred to as Over-the-air-updates (OTA) when
delivered through a network. But in some use cases also local update
mechanisms should be considered.

There are many ways to implement an update mechanism. EVerest supports
*RAUC* as an update tool, which has the following advantages:

-  Open source project with a large community:
   https://rauc.io
-  Secure by design: The update files are cryptographically signed (and
   optionally encrypted). Signature is checked during installation, so
   the source of the update file can be trusted. This simplifies the
   update delivery process a lot compared to other tools that only rely
   on transport mechanism security. Updates can be downloaded from a
   simple unencrypted HTTP server or even a local USB flash drive
   without compromising security.
-  Robust: Uses A/B partitioning and does full image updating
-  Atomic switching between A/B slots can be implemented
-  Support partial downloads by HTTP streaming: Block based partial
   downloads reduce the bandwidth needed

There are some considerations to make when choosing an update system:

+-----------------------------------+-----------------------------------+
| Full image updates                | Partial component / individual    |
|                                   | file updating                     |
+===================================+===================================+
| Very robust. The complete image   | Risk of producing an installed    |
| always has the correct            | combination where one component   |
| dependencies built in.            | is too old to work with the other |
|                                   | recently updated component.       |
|                                   | Requires careful tracking of      |
|                                   | compatibility between components. |
+-----------------------------------+-----------------------------------+
| KISS: Keep it simple and stupid.  | Often quite complex               |
| Writing full images to A/B slots  | implementations. That can         |
| is very simple. Combined with an  | introduce a lot of room for bugs  |
| atomic switch between the boot    | which brick devices during failed |
| slots, there is no critical time  | updates, power losses during      |
| where e.g. a power loss could     | updates or upgrading to           |
| brick a device.                   | incompatible updater software     |
|                                   | versions.                         |
+-----------------------------------+-----------------------------------+
| Simple versioning: a single       | Complex versioning: Always a      |
| version number is enough to       | combination of the different      |
| specify which software image      | components / files.               |
| version is installed.             |                                   |
+-----------------------------------+-----------------------------------+
| Recovers from file system errors  | Relies entirely on the filesystem |
| in the root partition: It writes  | implementation to repair itself   |
| a new clean FS on every update    | and may brick if that fails.      |
+-----------------------------------+-----------------------------------+
| Updates everything: rootfs,       | Often limited to e.g. application |
| kernel, bootloader, …             | update. It may e.g. not update    |
|                                   | kernel or base system.            |
+-----------------------------------+-----------------------------------+
| Downside: Full image updates      | Advantage: only download changed  |
| require more download             | files and thus have the smallest  |
| bandwidth/data. Can be mitigated  | possible download.                |
| to some extent by block based     |                                   |
| partial download.                 |                                   |
+-----------------------------------+-----------------------------------+

We chose RAUC as the most suitable update system for EVerest, mainly
due to its robust, brick-free mechanisms and its inherent security
features.

The larger download size is mitigated by using the HTTP streaming
feature, which only downloads the blocks that have changed instead of
the full image via HTTP range requests.

Security is provided on a block-based level, so there is no need to
first download the complete image and validate signature etc. It is done
on the fly.

This also means that no extra disk space is needed to store the update
image: It will be directly streamed from the source into the inactive
slot partition.

RAUC implementation in EVerest
------------------------------

EVerest implements everything down to the actual RAUC process in Linux: 
the interface is the D-Bus interface of RAUC. The implementation for
this is provided as part of the 
`Linux Systemd Rauc module </reference/modules/Linux_Systemd_Rauc>`_.

In EVerest the update process is fully integrated with OCPP.
In the OCPP use case, the CPO will need to provide storage for the
update file that is accessible via HTTP with range requests. The CSMS
then sends this URL in the update request to EVerest, and EVerest will
trigger RAUC on the D-Bus to actually perform the update.

You will need to implement the following in your Yocto system as this is
very system dependent:

-  A partitioning setup that provides A/B slots for rootfs, A/B boot and
   data partitions (more details about this is covered in the appendix!)
-  RAUC configuration file for your setup (system.conf)
-  RAUC backend that performs switching slots/marking good/bad. RAUC
   already comes with backend support for many bootloaders such as
   U-Boot and Barebox etc.
-  PKI to be used for signing / optionally encrypting the update files
-  A recipe that builds RAUC bundles (update files) directly in Yocto

If you use PHYTEC SoMs: Their *ampliPHY* distribution already has working
examples for all of the above in *ampliphy-rauc* or *ampliphy-secure*
distributions.

Refer to RAUC's integration documentation for more information:

https://rauc.readthedocs.io/en/latest/integration.html

One important aspect is how the switching between slots A and B is done.
This should ideally be done in an atomic way, otherwise, there is a
short moment in time where a power outage may lead to inconsistent state
for the boot configuration.

.. tip::

   Use *eMMC* as a storage device, as these provide a register that can be
   used to switch between the eMMC (hardware) boot partitions to select the
   A/B bootloader in an atomic way.

Test your integration locally first using RAUC on the command line:

.. tip::

   rauc install http://myudateserver.com/version1.raucb

RAUC should perform a successful installation on the currently unused
slot. Once that is done, issue a reboot and verify it cleanly boots into
the new slot.

Once booted successfully into the new slot, you need to mark the slot as
“good”, otherwise it will fall back to the previous one on the next
boot.

Some implementations do this in a *systemd* service that runs at the end
of the boot process. This is not recommended in production. EVerest
will take care of marking the slot as "good" when EVerest starts up
successfully. It will then also report the status to the OCPP backend
automatically etc.

To mark it "good", manually use:

::

   rauc status mark-good

You also may want to check RAUC's status before and after the update to
verify it is configured correctly. It shows an output like this:

::

   root@mysystem:~# rauc status
   === System Info ===
   Compatible:  mysystem-v1
   Variant:      
   Booted from: rootfs.0 (system0)

   === Bootloader ===
   Activated: rootfs.0 (system0)

   === Slot States ===
    [bootloader.0] (/dev/mmcblk1, boot-emmc, inactive)

   o [rootfs.1] (/dev/mmcblk1p6, ext4, inactive)
          bootname: system1
          boot status: good
      [boot.1] (/dev/mmcblk1p2, vfat, inactive)

   x [rootfs.0] (/dev/mmcblk1p5, ext4, booted)
          bootname: system0
          mounted: /
          boot status: good
      [boot.0] (/dev/mmcblk1p1, vfat, active)

Also try to use *mark-bad* and test if it falls back to the previous one
on the next boot.

EVerest will trigger RAUC via D-Bus, so make sure it is running as a
D-Bus service. The D-Bus interface is also the boundary between
EVerest and the underlying Linux system here.

Once you verified that RAUC performs updating and fall-backs in manually
controlled command line mode, you should be all set up for EVerest
updates.

Custom Update Mechanism
------------------------

In case you do not want to use RAUC and/or integrate your custom update
mechanism into EVerest, you can also implement the `EVerest System API
<TODO reference to external system API>`_. This would still 
allow you to update EVerest via OCPP, but you would need to handle the
actual update process yourself and provide status updates to EVerest via
the System API.

Optimize the base system
------------------------

If you have a lot of processes running in the Linux system and a very
high CPU load (which easily happens on small embedded systems), take
some time to select the correct nice levels for all services running on
the system. You can set the nice level in the systemd unit files.

.. tip::

   Being "nicer" means getting CPU less often if lots of processes are scheduled.

Especially for high-level communication (aka ISO 15118), run EVerest at
e.g. a nice level of -20 to ensure it is getting enough CPU slices
during the charging process. If you have other tasks outside of
EVerest, make sure they have a higher nice level.

Using a preemptive kernel is also a good idea to ensure low latencies in
user space. Check *CONF_PREEMT* documentation in the Linux kernel.

--------------------------------

Authors: Cornelius Claussen, Manuel Ziegler, Piet Gömpel
