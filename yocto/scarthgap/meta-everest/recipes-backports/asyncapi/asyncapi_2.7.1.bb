LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"

SRC_URI = "git://git@github.com/asyncapi/cli.git;protocol=ssh;branch=master"
# v2.7.1
SRCREV = "2e5c6cd224eef9f8924c7d7ec961b37fd0123f21"

S = "${WORKDIR}/git"

inherit npm

BBCLASSEXTEND = "native"
