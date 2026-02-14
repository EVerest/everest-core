LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"

SRC_URI = "git://github.com/EVerest/everest-examples.git;branch=main;protocol=https"

SRCREV = "459ef979771cc93a7c00d53188f813449b1e3bd6"

S = "${WORKDIR}/git"

inherit cmake pkgconfig python3native

DEPENDS = " \
    everest-core \
    evcli-native \
"

INSANE_SKIP:${PN} = "already-stripped useless-rpaths arch file-rdeps"
INSANE_SKIP:${PN}-dev = "already-stripped useless-rpaths arch file-rdeps"

FILES:${PN} += "${datadir}/everest/*"

EXTRA_OECMAKE += " \
    -DDISABLE_EDM=ON \
    -Deverest-examples_USE_PYTHON_VENV=OFF \
"

do_install:append() {
    # auto generated file that would conflict with the one created by everest-core
    rm -f ${D}${datadir}/everest/version_information.txt
}
