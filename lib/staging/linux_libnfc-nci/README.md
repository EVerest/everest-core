linux_libnfc-nci
================

branch NCI2.0_PN7160: Linux NFC stack for PN7160 NCI2.0 based NXP NFC Controller.
For previous NXP NFC Controllers support (PN7150, PN7120) refer to branch master.

Information about NXP NFC Controller can be found on [NXP website](https://www.nxp.com/products/identification-and-security/nfc/nfc-reader-ics:NFC-READER).

Further details about the stack [here](https://www.nxp.com/doc/AN13287).

Release version
---------------
- NCI2.0-R1.1 fix limitation about multiple T5T, fix issue with MIFARE Classic read after write, cleanup of alternate transport definition, cleanup of logs 
- NCI2.0-R1.0 is the first official release of Linux libnfc-nci stack for PN7160

Possible problems, known errors and restrictions of R1.1:
---------------------------------------------------------
- LLCP1.3 support requires OpenSSL Cryptography and SSL/TLS Toolkit (version 1.0.1j or later)
- To install on 64bit OS, apply patch

---

Modifications Notice
--------------------
The [original upstream source code](https://github.com/NXPNFCLinux/linux_libnfc-nci), released under the Apache 2.0 license and re-distributed here under the same license, has been modified as indicated below:

- Applied 64bit patch
- Changed runtime configuration (```conf/*```)
- Added CMakeLists.txt to build the library
- Applied patch below, in order to remove hard-coded config path dependency

```
--- a/linux_libnfc-nci/src/libnfc-utils/src/ConfigPathProvider.cc
+++ b/linux_libnfc-nci/src/libnfc-utils/src/ConfigPathProvider.cc
@@ -83,17 +83,17 @@
         << StringPrintf("%s: enter FileType:0x%02x", __func__, type);
     switch (type) {
     case VENDOR_NFC_CONFIG: {
-        string path = "//usr//local//etc//libnfc-nxp.conf";
+        string path = CONFIG_PATH "libnfc-nxp.conf";
         addEnvPathIfAvailable(path);
         return path;
     } break;
     case VENDOR_ESE_CONFIG: {
-        string path = "//usr//local//etc//libese-nxp.conf";
+        string path = CONFIG_PATH "libese-nxp.conf";
         addEnvPathIfAvailable(path);
         return path;
     } break;
     case SYSTEM_CONFIG: {
-        const vector<string> searchPath = { "//usr/local//etc//" };
+        const vector<string> searchPath = { CONFIG_PATH };
         for (string path : searchPath) {
             addEnvPathIfAvailable(path);
             path.append("libnfc-nci.conf");
@@ -105,12 +105,12 @@
         return "";
     } break;
     case RF_CONFIG: {
-        string path = "//usr//local//etc//libnfc-nxp_RF.conf";
+        string path = CONFIG_PATH "libnfc-nxp_RF.conf";
         addEnvPathIfAvailable(path);
         return path;
     } break;
     case TRANSIT_CONFIG: {
-        string path = "//usr/local//etc//libnfc-nxpTransit.conf";
+        string path = CONFIG_PATH "libnfc-nxpTransit.conf";
         addEnvPathIfAvailable(path);
         return path;
     } break;
@@ -121,22 +121,22 @@
         return nfc_storage_path;
     } break;
     case FIRMWARE_LIB: {
-        string path = "//usr/local//etc//libsn100u_fw.dll";
+        string path = CONFIG_PATH "libsn100u_fw.dll";
         addEnvPathIfAvailable(path);
         return path;
     } break;
     case CONFIG_TIMESTAMP: {
-        string path = "//usr//local//etc//libnfc-nxpConfigState.bin";
+        string path = CONFIG_PATH "libnfc-nxpConfigState.bin";
         addEnvPathIfAvailable(path);
         return path;
     } break;
     case RF_CONFIG_TIMESTAMP: {
-        string path = "//usr//local//etc//libnfc-nxpRFConfigState.bin";
+        string path = CONFIG_PATH "libnfc-nxpRFConfigState.bin";
         addEnvPathIfAvailable(path);
         return path;
     } break;
     case TRANSIT_CONFIG_TIMESTAMP: {
-        string path = "//usr//local//etc//libnfc-nxpTransitConfigState.bin";
+        string path = CONFIG_PATH "libnfc-nxpTransitConfigState.bin";
         addEnvPathIfAvailable(path);
         return path;
     } break;
```
