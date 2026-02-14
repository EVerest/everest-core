inherit pypi
inherit python3native python3targetconfig

PYPI_PACKAGE ?= "${@pypi_package(d)}"

# using debian mirror here, because of easy url
def pypi_wheel_uri(d):
    package = d.getVar('PYPI_PACKAGE')
    artifact_name = d.getVar('PYPI_WHEEL_NAME')
    return 'https://pypi.debian.net/%s/%s' % (package, artifact_name)

PYPI_WHEEL_URI ?= "${@pypi_wheel_uri(d)};name=wheel"

SRC_URI:prepend = "${PYPI_WHEEL_URI} "

FILES:${PN} += "${libdir}/* ${libdir}/${PYTHON_DIR}/*"

FILES:${PN}-staticdev += "\
  ${PYTHON_SITEPACKAGES_DIR}/*.a \
"
FILES:${PN}-dev += "\
  ${datadir}/pkgconfig \
  ${libdir}/pkgconfig \
  ${PYTHON_SITEPACKAGES_DIR}/*.la \
"

DEPENDS:append = " python3-installer-native"

# pypa/installer option to control the bytecode compilation
INSTALL_WHEEL_COMPILE_BYTECODE ?= "--compile-bytecode=0"

pypi_wheel_do_install() {
    nativepython3 -m installer ${INSTALL_WHEEL_COMPILE_BYTECODE} \
    --interpreter "${USRBINPATH}/env python3" \
    --destdir=${D} ${WORKDIR}/${PYPI_WHEEL_NAME}
}

EXPORT_FUNCTIONS do_install
