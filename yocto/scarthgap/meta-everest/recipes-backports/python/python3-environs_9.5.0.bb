SUMMARY = "environs: simplified environment variable parsing"
    
HOMEPAGE = "https://github.com/sloria/environs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=a49f11890d809ccbf7d326a2df842aaf"

SRC_URI[md5sum] = "7df5cb194ff30a004e1f03d9b4fdbee6"

PYPI_PACKAGE = "environs"   

inherit pypi setuptools3

RDEPENDS:${PN} += " \
    ${PYTHON_PN}-python-dotenv \
"
