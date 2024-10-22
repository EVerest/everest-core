lib_LTLIBRARIES = libnfc_nci_linux.la

sbin_PROGRAMS = nfcDemoApp
nfcDemoApp_DEPENDENCIES = libnfc_nci_linux.la
configdir = ${sysconfdir}

config_DATA = conf/libnfc-nci.conf conf/libnfc-nxp.conf

pkgconfiglibdir = $(libdir)/pkgconfig

pkgconfiglib_DATA = \
	libnfc-nci.pc

AM_CPPFLAGS = \
	-DCONFIG_PATH='"$(configdir)/"' \
	-I$(srcdir)/src/include \
	$(INCLUDE_PARAMS) \
	$(libnfc_nci_linux_la_FLAGS) \
	-DNXP_EXTNS=TRUE \
	-DLINUX \
	-DSNEP_ENABLED

AM_LDFLAGS = $(LDFLAGS) -Bstatic

nfcDemoApp_FLAGS = -I$(srcdir)/demoapp -I$(srcdir)/src/include

if LLCP1_3
AM_CPPFLAGS += \
	-I$(openssldir)/include
AM_LDFLAGS += -L$(openssldir)/lib -lcrypto -lssl
endif

lib_LTLIBRARIES += libpn7160_fw.la

libpn7160_fw_la_SOURCES = \
	firmware/pn7160/phDnldNfc_UpdateSeq.cc \
	firmware/pn7160/phDnldNfc_ImageInfo.cc

AM_CPPFLAGS += -I$(srcdir)/firmware/pn7160

libpn7160_fw_la_LDFLAGS = -shared
LIBNFC_NCI_INCLUDE := \
	src/nxp_nci_hal_libnfc-nci/src/include \
	src/nxp_nci_hal_libnfc-nci/src/ \
	src/nxp_nci_hal_libnfc-nci/src/gki/ulinux \
	src/nxp_nci_hal_libnfc-nci/src/gki/common \
	src/nxp_nci_hal_libnfc-nci/src/nfa/include \
	src/nxp_nci_hal_libnfc-nci/src/nfa/int \
	src/nxp_nci_hal_libnfc-nci/src/nfc/include \
	src/nxp_nci_hal_libnfc-nci/src/nfc/int \
	src/nxp_nci_hal_libnfc-nci/src/hal/include \
	src/nxp_nci_hal_libnfc-nci/src/hal/int \
	src/nxp_nci_hal_libnfc-nci/src/fuzzers/inc/android/hardware/nfc/1.1/ \
	src/android/utility/ \
	src/nfcandroid_nfc_hidlimpl/halimpl/utils/ \
	src/libnfc-utils/inc \
        src/nfcandroid_nfc_hidlimpl/halimpl/common/ \
	src/android/utility/hardware/\
	src/android/utility/android/

LIBNFC_NCI_SOURCE := \
	src/android/utility/base/file.cpp \
	src/libnfc-utils/src/ConfigPathProvider.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/rw/nfa_rw_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/rw/nfa_rw_api.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/rw/nfa_rw_act.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/rw_mfc.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/hci/nfa_hci_ci.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/hci/nfa_hci_api.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/hci/nfa_hci_act.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/hci/nfa_hci_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/hci/nfa_hci_utils.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/ce/nfa_ce_act.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/ce/nfa_ce_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/ce/nfa_ce_api.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/p2p/nfa_p2p_act.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/p2p/nfa_p2p_api.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/p2p/nfa_p2p_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/sys/nfa_sys_cback.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/sys/nfa_sys_ptim.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/sys/nfa_sys_cfg.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/sys/nfa_sys_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/dm/nfa_dm_discover.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/dm/nfa_dm_act.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/dm/nfa_dm_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/dm/nfa_dm_ndef.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/dm/nfa_dm_api.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/dm/nfa_dm_cfg.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/ee/nfa_ee_api.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/ee/nfa_ee_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/ee/nfa_ee_act.cc \
	src/nxp_nci_hal_libnfc-nci/src/adaptation/libmain.cc \
	src/nxp_nci_hal_libnfc-nci/src/gki/common/gki_debug.cc \
	src/nxp_nci_hal_libnfc-nci/src/gki/common/gki_time.cc \
	src/nxp_nci_hal_libnfc-nci/src/gki/common/gki_buffer.cc \
	src/nxp_nci_hal_libnfc-nci/src/gki/ulinux/gki_ulinux.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/llcp/llcp_sdp.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/llcp/llcp_link.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/llcp/llcp_api.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/llcp/llcp_util.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/llcp/llcp_dlc.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/llcp/llcp_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/nfc/nfc_vs.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/nfc/nfc_task.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/nfc/nfc_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/nfc/nfc_ncif.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/nfc/nfc_test.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/nfc/nfc_ee.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/nfc/nfc_utils.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/nci/nci_hrcv.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/nci/nci_hmsgs.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/rw_i93.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/rw_t4t.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/tags_int.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/ce_t3t.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/rw_t2t.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/rw_t2t_ndef.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/rw_t1t.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/ce_t4t.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/rw_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/rw_t3t.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/ce_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/tags/rw_t1t_ndef.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfc/ndef/ndef_utils.cc \
	src/nxp_nci_hal_libnfc-nci/utils/config.cc \
	src/nxp_nci_hal_libnfc-nci/src/adaptation/CrcChecksum.cpp \
	src/nxp_nci_hal_libnfc-nci/src/adaptation/NfcAdaptation.cc \
	src/libnfc-utils/src/android_logmsg.cpp \
	src/nxp_nci_hal_libnfc-nci/src/adaptation/nfc_config.cc \
    src/nxp_nci_hal_libnfc-nci/utils/ringbuffer.cc \
    src/libnfc-utils/cho/ndef_cho_utils.cc \
	src/libnfc-utils/src/OverrideLog.cpp \
	src/android/utility/base/phNxpLogging.cpp \
	src/android/utility/base/strings.cpp \
	src/android/utility/android-base/stringprintf.cpp \
	src/nxp_nci_hal_libnfc-nci/src/nfa/t4tnfcee/nfa_nfcee_main.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/t4tnfcee/nfa_nfcee_api.cc \
	src/nxp_nci_hal_libnfc-nci/src/nfa/t4tnfcee/nfa_nfcee_act.cc

HALIMPL_INCLUDE := \
	src/nxp_nci_hal_libnfc-nci/src/gki/ulinux \
	src/nxp_nci_hal_libnfc-nci/src/include \
	src/nfcandroid_nfc_hidlimpl/halimpl/utils/ \
	src/nfcandroid_nfc_hidlimpl/halimpl/inc/ \
	src/nfcandroid_nfc_hidlimpl/halimpl/common/ \
	src/nfcandroid_nfc_hidlimpl/halimpl/dnld/ \
	src/nfcandroid_nfc_hidlimpl/halimpl/hal/ \
	src/nfcandroid_nfc_hidlimpl/halimpl/log/ \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml \
	src/nxp_nci_hal_libnfc-nci/src/fuzzers/inc/android/hardware \
	src/nfcandroid_nfc_hidlimpl/halimpl/self-test\
	src/nfcandroid_nfc_hidlimpl/halimpl/mifare \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/transport \
	src/nfcandroid_nfc_hidlimpl/halimpl/src/include \
	src/nxp_nci_hal_libnfc-nci/src/fuzzers/inc/ \
	src/nxp_nci_hal_libnfc-nci/utils/include \
	src/nxp_nci_hal_libnfc-nci/utils \
	src/nfcandroid_nfc_hidlimpl/extns/impl/ \
	src/android/secure_element/extns/impl/ \
	src/android/utility/base/ \
	src/libnfc-utils/inc

HALIMPL_SOURCE := \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/phDal4Nfc_messageQueueLib.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/phOsalNfc_Timer.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/phTmlNfc.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/dnld/phDnldNfc.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/dnld/phNxpNciHal_Dnld.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/dnld/phDnldNfc_Internal.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/dnld/phDnldNfc_Utils.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/log/phNxpLog.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/self-test/phNxpNciHal_SelfTest.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/hal/phNxpNciHal_NfcDepSWPrio.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/hal/phNxpNciHal_dta.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/hal/phNxpNciHal_ext.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/hal/phNxpNciHal.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/utils/phNxpNciHal_utils.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/utils/phNxpConfig.cpp \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/NfccTransportFactory.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/utils/NfccPowerTracker.cpp \
	src/nfcandroid_nfc_hidlimpl/halimpl/mifare/NxpMfcReader.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/utils/NxpNfcCapability.cpp \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/transport/NfccI2cTransport.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/transport/NfccAltI2cTransport.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/transport/NfccAltSpiTransport.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/transport/NfccAltTransport.cc \
	src/nfcandroid_nfc_hidlimpl/halimpl/tml/transport/NfccTransport.cc

SERVICE_INCLUDE := \
	src/service/interface \
	src/service/utils \
	src/nxp_nci_hal_nfc/nci/jni/extns/pn54x/inc \
	src/nxp_nci_hal_nfc/nci/jni/extns/pn54x/src/common \
	src/nxp_nci_hal_nfc/nci/jni/extns/pn54x/src/mifare \
	src/nxp_nci_hal_nfc/nci/jni/ \
	src/nfcandroid_nfc_hidlimpl/halimpl/log


SERVICE_SOURCE := \
	src/nxp_nci_hal_nfc/nci/jni/Mutex.cpp \
	src/nxp_nci_hal_nfc/nci/jni/IntervalTimer.cpp \
	src/nxp_nci_hal_nfc/nci/jni/CondVar.cpp \
	src/nxp_nci_hal_nfc/nci/jni/NativeNfcTag.cpp \
	src/nxp_nci_hal_nfc/nci/jni/NfcTag.cpp \
	src/nxp_nci_hal_nfc/nci/jni/NativeNxpFeature.cpp \
	src/nxp_nci_hal_nfc/nci/jni/NativeT4tNfcee.cpp \
	src/nxp_nci_hal_nfc/nci/jni/NativeT4tNfceeManager.cpp \
	src/service/interface/nativeNdef.cpp \
	src/nxp_nci_hal_nfc/nci/jni/RoutingManager.cpp \
	src/nxp_nci_hal_nfc/nci/jni/extns/pn54x/src/mifare/phFriNfc_SmtCrdFmt.cpp \
	src/nxp_nci_hal_nfc/nci/jni/extns/pn54x/src/mifare/phNxpExtns_MifareStd.cpp \
	src/nxp_nci_hal_nfc/nci/jni/extns/pn54x/src/mifare/phFriNfc_MifStdFormat.cpp \
	src/nxp_nci_hal_nfc/nci/jni/extns/pn54x/src/mifare/phFriNfc_MifareStdMap.cpp \
	src/nxp_nci_hal_nfc/nci/jni/extns/pn54x/src/phNxpExtns.cpp \
	src/service/linux_nfc_api.cpp \
	src/service/linux_nfc_factory_api.cpp \
	src/service/interface/nativeNfcSnep.cpp \
	src/libnfc-utils/src/nfa_snep_api.cpp \
	src/libnfc-utils/src/nfa_snep_act.cpp \
	src/libnfc-utils/src/nfa_snep_main.cpp \
	src/libnfc-utils/src/nfa_snep_default.cpp \
	src/service/interface/nativeNfcHandover.cpp \
	src/service/interface/nativeNfcLlcp.cpp \
	src/nxp_nci_hal_nfc/nci/jni/NativeNfcManager.cpp \
	src/nxp_nci_hal_nfc/nci/jni/PeerToPeer.cpp


nfcDemoApp_SOURCES := \
	demoapp/main.c \
	demoapp/tools.c


libnfc_nci_linux_la_SOURCES = \
	$(LIBNFC_NCI_SOURCE) \
	$(HALIMPL_SOURCE) \
	$(SERVICE_SOURCE)

include_HEADERS = \
	src/include/linux_nfc_api.h \
	src/include/linux_nfc_factory_api.h \
	src/include/linux_nfc_api_compatibility.h

INCLUDE_PARAMS :=$(LIBNFC_NCI_INCLUDE:%=-I$(srcdir)/%)
INCLUDE_PARAMS +=$(HALIMPL_INCLUDE:%=-I$(srcdir)/%)
INCLUDE_PARAMS +=$(SERVICE_INCLUDE:%=-I$(srcdir)/%)

libnfc_nci_linux_la_FLAGS =

if DEBUG
libnfc_nci_linux_la_FLAGS += -DDEBUG
else
libnfc_nci_linux_la_FLAGS += -DNDEBUG
endif

libnfc_nci_linux_la_FLAGS += -pthread -w
libnfc_nci_linux_la_FLAGS += -DNFC_NXP_NOT_OPEN_INCLUDED=TRUE
libnfc_nci_linux_la_FLAGS += -DNXP_HW_SELF_TEST
libnfc_nci_linux_la_FLAGS += -DNXP_NFC_NATIVE_ENABLE_HCE=TRUE

if LLCP1_3
libnfc_nci_linux_la_FLAGS += -DNFC_NXP_LLCP_SECURED_P2P=TRUE
else
libnfc_nci_linux_la_FLAGS += -DNFC_NXP_LLCP_SECURED_P2P=FALSE
endif

libnfc_nci_linux_la_FLAGS += -DPH_NCI_NXP_HAL_ENABLE_FW_DOWNLOAD=FALSE
libnfc_nci_linux_la_FLAGS += -DNXP_NEW_MW_API_COMPATIBILITY

libnfc_nci_linux_la_LIBADD = $(AC_LIBOBJS)
libnfc_nci_linux_la_LDFLAGS := $(AM_LDFLAGS)
libnfc_nci_linux_la_LDFLAGS += -shared -pthread -ldl -lrt -fPIC -release 1 -version-info 0:0:0

nfcDemoApp_LDFLAGS = -pthread -ldl -lrt -lnfc_nci_linux

