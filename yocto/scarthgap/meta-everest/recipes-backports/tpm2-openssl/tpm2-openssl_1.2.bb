
SUMMARY = "Provider for integration of TPM 2.0 to OpenSSL 3.0"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b75785ac083d3c3ca04d99d9e4e1fbab"

DEPENDS = "autoconf-archive-native tpm2-tss openssl"

SRC_URI = "git://github.com/tpm2-software/tpm2-openssl.git;protocol=ssh;branch=master"
SRCREV = "d0ec28709b74f9671274bdc9b0f5aac69f5aef67"

S = "${WORKDIR}/git"

inherit autotools pkgconfig

do_configure:prepend() {
    # do not extract the version number from git
    sed -i -e 's/m4_esyscmd_s(\[git describe --tags --always --dirty\])/${PV}/' ${S}/configure.ac
}

FILES:${PN} = "\
    ${libdir}/ossl-modules/tpm2.so"
