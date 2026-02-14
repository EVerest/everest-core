LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"

require everest-core_git.inc

SRC_URI:append = " file://everest.service"

do_compile[network] = "0"

inherit cmake pkgconfig systemd python3native python3targetconfig

DEPENDS = " \
    boost \
    curl \
    date \
    evcli-native \
    everest-cmake \
    fmt \
    ftxui \
    json-schema-validator \
    libcap \
    libevent \
    libnfc-nci \
    libpcap \
    libwebsockets \
    mosquitto \
    mqttc \
    nlohmann-json \
    openssl \
    pugixml \
    rapidyaml \
    rsync-native \
    sdbus-c++ \
    sigslot \
"

RDEPENDS:${PN} += "libevent openssl"

INSANE_SKIP:${PN} = "already-stripped useless-rpaths arch file-rdeps"

FILES:${PN} += "${libdir}/everest/* ${datadir}/everest/*"

EXTRA_OECMAKE += " \
    -DDISABLE_EDM=ON \
    -DNO_FETCH_CONTENT=ON \
    -DEVEREST_ENABLE_RUN_SCRIPT_GENERATION=OFF \
    -Deverest-core_INSTALL_EV_CLI_IN_PYTHON_VENV=OFF \
    -Deverest-core_USE_PYTHON_VENV=OFF \
    -DEV_SETUP_PYTHON_EXECUTABLE_USE_PYTHON_VENV=OFF \
    -DPYTHON_MODULE_EXTENSION=.so \
    -DPYBIND11_PYTHONLIBS_OVERWRITE=OFF \
    -DEVEREST_INSTALL_ADMIN_PANEL=OFF \
    -DLOG_INSTALL=ON \
    -DEVEREST_SQLITE_INSTALL=ON \
    -DFRAMEWORK_INSTALL=ON \
    -DTIMER_INSTALL=ON \
    -DEVSE_SECURITY_INSTALL=ON \
    -DOCPP_INSTALL=ON \
"

SYSTEMD_SERVICE:${PN} = "everest.service"

PACKAGECONFIG ??= "admin-panel applications python ${@bb.utils.filter('DISTRO_FEATURES', 'tpm2', d)}"

PACKAGECONFIG[admin-panel] = "-DEVEREST_ENABLE_ADMIN_PANEL_BACKEND=ON,-DEVEREST_ENABLE_ADMIN_PANEL_BACKEND=OFF,"
PACKAGECONFIG[applications] = "-DEVEREST_BUILD_APPLICATIONS=ON,-DEVEREST_BUILD_APPLICATIONS=OFF,"
PACKAGECONFIG[javascript] = "-DEVEREST_ENABLE_JS_SUPPORT=ON,-DEVEREST_ENABLE_JS_SUPPORT=OFF,nodejs-native"
PACKAGECONFIG[python] = "-DEVEREST_ENABLE_PY_SUPPORT=ON,-DEVEREST_ENABLE_PY_SUPPORT=OFF,python3-pybind11 python3-pybind11-json"
PACKAGECONFIG[tpm2] = "-DUSING_TPM2=ON,-DUSING_TPM2=OFF,"

do_install:append() {
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_system_unitdir}
        install -m 0644 ${WORKDIR}/everest.service ${D}${systemd_system_unitdir}/
    fi
}

OECMAKE_CXX_FLAGS += "-Wno-narrowing"
