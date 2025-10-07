#
# AUTO GENERATED - MARKED REGIONS WILL BE KEPT
# template version 3
#

# module setup:
#   - ${MODULE_NAME}: module name
ev_setup_cpp_module()

# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1
# insert your custom targets and additional config variables here
target_link_libraries(${MODULE_NAME}
    PRIVATE
        everest::run_application
)
# ev@bcc62523-e22b-41d7-ba2f-825b493a3c97:v1

target_sources(${MODULE_NAME}
    PRIVATE
        "iso15118_vas/ISO15118_vasImpl.cpp"
)

# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
# insert other things like install cmds etc here
# Check for radvd and udhcpd executables
find_program(RADVD_EXECUTABLE radvd)
if(NOT RADVD_EXECUTABLE)
    message(WARNING "radvd executable not found. The Iso15118InternetVas module will not work correctly.")
endif()

find_program(UDHCPD_EXECUTABLE udhcpd)
if(NOT UDHCPD_EXECUTABLE)
    message(WARNING "udhcpd executable not found. The Iso15118InternetVas module will not work correctly.")
endif()

install(
    PROGRAMS
        vas-internet-setup.sh
    DESTINATION "${EVEREST_MODULE_INSTALL_PREFIX}/${MODULE_NAME}"
)
# ev@c55432ab-152c-45a9-9d2e-7281d50c69c3:v1
