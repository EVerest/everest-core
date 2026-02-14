SUMMARY = "C++ functional terminal UI"
DESCRIPTION = "{ftxui}: A simple cross-platform C++ library for terminal based user interfaces!"
HOMEPAGE = "https://arthursonzogni.github.io/FTXUI"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=602507f167b627b30ce2cd7a24d50ea3"

SRC_URI = "git://github.com/ArthurSonzogni/FTXUI.git;branch=main;protocol=https"
SRCREV = "5cfed50702f52d51c1b189b5f97f8beaf5eaa2a6"

S = "${WORKDIR}/git"

inherit cmake

EXTRA_OECMAKE += " -DBUILD_SHARED_LIBS=ON "

BBCLASSEXTEND = "native nativesdk"
