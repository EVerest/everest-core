LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"

# ev-cli move to everest-core, include the version from there in this recipe
require ../everest/everest-core_git.inc

SETUPTOOLS_SETUP_PATH = "${S}/applications/utils/ev-dev-tools"

inherit setuptools3

do_configure:prepend() {
cat > ${SETUPTOOLS_SETUP_PATH}/setup.py <<-EOF
from setuptools import setup

setup()
EOF
}

DEPENDS = "python3-pip-native"

RDEPENDS:${PN} = " \
    pip-stringcase \
    python3-jsonschema \
    python3-pyyaml \
    python3-jinja2 \
"

BBCLASSEXTEND = "native"
BBCLASSEXTEND:append = " nativesdk"
