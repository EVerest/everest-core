DESCRIPTION = "Python implementation of the ISO 15118-2 and -20 protocols"
HOMEPAGE = "https://github.com/SwitchEV/iso15118"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"

SRC_URI = "git://github.com/EVerest/ext-switchev-iso15118.git;protocol=https;branch=everest"

SRCREV = "89db9d27db551801d45627f802e4c3a029ed29c2"

S = "${WORKDIR}/git"

inherit python_poetry_core

# NOTE (aw): the cryptography dependency might be to old
RDEPENDS:${PN} += " \
    ${PYTHON_PN}-environs \
    ${PYTHON_PN}-cryptography \
    ${PYTHON_PN}-pydantic \
    ${PYTHON_PN}-psutil \
    ${PYTHON_PN}-py4j \
    ${PYTHON_PN}-marshmallow \
    ${PYTHON_PN}-aiofile \
    ${PYTHON_PN}-netifaces \
"

# iso15118 ships shell scripts which use /bin/bash
RDEPENDS:${PN} += "bash"
