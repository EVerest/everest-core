SUMMARY = "marshmallow: A lightweight library for converting complex datatypes to and from native Python datatypes."

HOMEPAGE = "https://github.com/marshmallow-code/marshmallow"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=5bccd400dedfa74364481e56aacc0b4a"

SRC_URI[sha256sum] = "5d2371bbe42000f2b3fb5eaa065224df7d8f8597bc19a1bbfa5bfe7fba8da889"

PYPI_PACKAGE = "marshmallow"

inherit pypi setuptools3

RDEPENDS:${PN} += " \
    ${PYTHON_PN}-packaging \
"
