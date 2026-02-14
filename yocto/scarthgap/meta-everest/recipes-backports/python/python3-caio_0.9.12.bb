SUMMARY = "caio: Asynchronous file IO for Linux MacOS or Windows."

HOMEPAGE = "https://github.com/mosquito/caio"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=e3fc50a88d0a364313df4b21ef20c29e"

SRC_URI[sha256sum] = "d2be553738dd793f8a01a60316f2c5284fbf152219241c0c67ca05f650a37a37"

PYPI_PACKAGE = "caio"

inherit pypi setuptools3

RDEPENDS:${PN} += "python3-multiprocessing"
