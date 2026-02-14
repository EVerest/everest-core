SUMMARY = "typing-extensions: Backported and Experimental Type Hints for Python 3.7+"

HOMEPAGE = "https://github.com/python/typing_extensions"
LICENSE = "PSF-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=fcf6b249c2641540219a727f35d8d2c2"

SRC_URI[sha256sum] = "b75ddc264f0ba5615db7ba217daeb99701ad295353c45f9e95963337ceeeffb2"

PYPI_PACKAGE = "typing_extensions"

inherit pypi setuptools3 python3-dir

do_configure:prepend() {
cat > setup.py <<-EOF
from setuptools import setup

setup(name='typing_extensions',
      version="4.7.1",
      package_dir={'': 'src'})
EOF
}

FILES:${PN} += "\
    ${libdir}/${PYTHON_DIR}/site-packages/typing_extensions.py \
"

do_install:append() {
    cp ${S}/src/typing_extensions.py ${D}${libdir}/${PYTHON_DIR}/site-packages/
}
