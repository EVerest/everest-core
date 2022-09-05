set(PACKAGE_VERSION 0.1)

if(PACKAGE_FIND_VERSION STREQUAL PACKAGE_VERSION)
    set(PACKAGE_VERSION_EXACT TRUE)
else ()
    # NOTE (aw): for now we don't have any versioning schema
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
endif()

