SUMMARY = "Enables Python programs to dynamically access arbitrary Java objects"
    
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=92361a2681db50d1bc207c47c49c1a8e"

SRC_URI[md5sum] = "22345a5235af2e003da25554732f2193"
SRC_URI[wheel.md5sum] = "8667328e99732d4577eeb85d1ded8db8"

PYPI_PACKAGE = "py4j"   
PYPI_WHEEL_NAME = "py4j-0.10.9.7-py2.py3-none-any.whl"   

inherit pypi_wheel

FILES:${PN} += "${datadir}/*"
