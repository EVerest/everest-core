LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"

FILESEXTRAPATHS:prepend := "${EVEREST_CORE_PATH}/..:"
SRC_URI = "file://everest-core;unpack=0 \
           "

S = "${WORKDIR}/everest-core"
B = "${WORKDIR}"
