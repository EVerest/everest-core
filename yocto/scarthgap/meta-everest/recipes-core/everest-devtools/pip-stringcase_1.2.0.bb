# The is automatic generated Code by "makePipRecipes.py"
# (build by Robin Sebastian (https://github.com/robseb) (git@robseb.de) Vers.: 1.2) 

SUMMARY = "Recipe to embedded the Python PiP Package stringcase"
HOMEPAGE ="https://pypi.org/project/stringcase"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=59260a4045da59ac7cb0820ac544b150"

inherit pypi setuptools3_legacy
PYPI_PACKAGE = "stringcase"
SRC_URI[md5sum] = "5cb2a0b28f227f19dc830b66f6e46b52"
SRC_URI[sha256sum] = "48a06980661908efe8d9d34eab2b6c13aefa2163b3ced26972902e3bdfd87008"
BBCLASSEXTEND = "native"
BBCLASSEXTEND:append = " nativesdk"
