SUMMARY = "Provider for integration of TPM 2.0 to OpenSSL 3.X"
DESCRIPTION = "The tpm2-openssl project implements a provider \
that integrates the Trusted Platform Module (TPM 2.0) operations \
to the OpenSSL 3.x, which is the next version of OpenSSL after 1.1.1."
HOMEPAGE = "https://github.com/tpm2-software/tpm2-openssl"
SECTION = "tpm"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3f4b4cb00f4d0d6807a0dc79759a57ac"

SRC_URI = "https://github.com/tpm2-software/${BPN}/releases/download/${PV}/${BPN}-${PV}.tar.gz"

SRC_URI[sha256sum] = "9a9aca55d4265ec501bcf9c56d21d6ca18dba902553f21c888fe725b42ea9964"

UPSTREAM_CHECK_URI = "https://github.com/tpm2-software/${BPN}/releases"
UPSTREAM_CHECK_REGEX = "releases/tag/v?(?P<pver>\d+(\.\d+)+)"

DEPENDS = "autoconf-archive-native tpm2-tss openssl"

inherit autotools pkgconfig

FILES:${PN} = "${libdir}/ossl-modules/*"
