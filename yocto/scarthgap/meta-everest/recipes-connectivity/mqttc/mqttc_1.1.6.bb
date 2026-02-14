SUMMARY = "A portable MQTT C client for embedded systems and PCs alike"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://LICENSE;md5=9226377baf0b79174c89a1ab55592456"

SRC_URI = "git://github.com/LiamBindle/MQTT-C;protocol=http;branch=master;protocol=https \
           file://0001-Add-cmake-package-config.patch \
           "

SRCREV = "f69ce1e7fd54f3b1834c9c9137ce0ec5d703cb4d"

S = "${WORKDIR}/git"

inherit cmake
