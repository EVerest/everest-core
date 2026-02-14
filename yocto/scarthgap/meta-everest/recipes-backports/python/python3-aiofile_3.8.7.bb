SUMMARY = "aiofile: Asynchronous file operations."

HOMEPAGE = "http://github.com/mosquito/aiofile"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENCE;md5=d8d1d59c60e60e8627fcd1c350a5c904"

SRC_URI[sha256sum] = "a8f9dec17282b3583337c4ef2d1a67f33072ab80dd03608041ed9e71b88dc521"

PYPI_PACKAGE = "aiofile"

inherit pypi setuptools3

RDEPENDS:${PN} += " \
    ${PYTHON_PN}-caio \
"
