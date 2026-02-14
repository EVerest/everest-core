SUMMARY = "A date and time library based on C++11/14/17."
AUTHOR = "Howard Hinnant"
HOMEPAGE = "https://github.com/HowardHinnant/date.git"
SECTION = "libs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=b5d973344b3c7bbf7535f0e6e002d017"

SRC_URI = "git://github.com/HowardHinnant/date.git;branch=master;protocol=https \
           "

inherit cmake

S = "${WORKDIR}/git"
PV = "3.0.4"

SRCREV = "f94b8f36c6180be0021876c4a397a054fe50c6f2"

EXTRA_OECMAKE += " \
	-DBUILD_TZ_LIB=ON \
	-DHAS_REMOTE_API=OFF \
	-DUSE_AUTOLOAD=OFF \
	-DUSE_SYSTEM_TZ_DB=ON \
	-DBUILD_SHARED_LIBS=ON \
"
