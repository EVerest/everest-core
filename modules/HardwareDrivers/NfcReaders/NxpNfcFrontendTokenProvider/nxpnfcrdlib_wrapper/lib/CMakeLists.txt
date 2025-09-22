cmake_minimum_required(VERSION 3.15)

set(DAL_PATH ${PATH_NXPNFCRDLIB}/Platform/DAL)
set(PHOSAL_PATH ${PATH_NXPNFCRDLIB}/RTOS/phOsal)
set(NXPLIB_PATH ${PATH_NXPNFCRDLIB}/NxpNfcRdLib)


# NOTE: The original CMake build files from NXP's NFC READER LIB are ignored.
#       The build is defined completely in the 'customization' folder:

add_subdirectory(customization)

