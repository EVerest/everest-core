SUMMARY = "JSON for modern C++"
HOMEPAGE = "https://nlohmann.github.io/json/"
SECTION = "libs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE.MIT;md5=3b489645de9825cca5beeb9a7e18b6eb"

CVE_PRODUCT = "json-for-modern-cpp"

SRC_URI = "git://github.com/nlohmann/json.git;nobranch=1;protocol=https \
           "

SRCREV = "55f93686c01528224f448c19128836e7df245f72"

S = "${WORKDIR}/git"

inherit cmake

EXTRA_OECMAKE += "-DJSON_BuildTests=OFF -DJSON_MultipleHeaders=ON"

# nlohmann-json is a header only C++ library, so the main package will be empty.

RDEPENDS:${PN}-dev = ""

BBCLASSEXTEND = "native nativesdk"

# other packages commonly reference the file directly as "json.hpp"
# create symlink to allow this usage
do_install:append() {
    ln -s nlohmann/json.hpp ${D}${includedir}/json.hpp
}
