# download everest-core (source of libcbv2g)
include(ExternalProject)
ExternalProject_Add(
    everest-core-src
    DOWNLOAD_DIR "everest-core/src"
    GIT_REPOSITORY "https://github.com/EVerest/everest-core.git"
    GIT_TAG "61e97863fd2fc9f429f9cd2e4b689139e7d46981"
    TIMEOUT 30
    LOG_DOWNLOAD ON
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

# build everest-core/lib/everest/cbv2g
ExternalProject_Add(
    cbv2g-src
    DOWNLOAD_COMMAND ""
    SOURCE_DIR "everest-core-src-prefix/src/everest-core-src/lib/everest/cbv2g"
    PREFIX "everest-core"
    INSTALL_COMMAND ""
    LOG_CONFIGURE ON
    LOG_BUILD ON
    DEPENDS everest-core-src
)
ExternalProject_Get_Property(cbv2g-src SOURCE_DIR)
ExternalProject_Get_Property(cbv2g-src BINARY_DIR)
set(CBV2G_INCLUDE_DIR "${SOURCE_DIR}/include")
set(CBV2G_LIB_DIR "${BINARY_DIR}/lib/cbv2g")

# workaround for https://gitlab.kitware.com/cmake/cmake/-/issues/15052
# create CBV2G_INCLUDE_DIR since it will not exist in configure step
file(MAKE_DIRECTORY ${CBV2G_INCLUDE_DIR})

add_library(cbv2g_tp STATIC IMPORTED)
add_library(cbv2g::tp ALIAS cbv2g_tp)
set_property(TARGET cbv2g_tp PROPERTY IMPORTED_LOCATION ${CBV2G_LIB_DIR}/libcbv2g_tp.a)
set_property(TARGET cbv2g_tp PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CBV2G_INCLUDE_DIR})
add_dependencies(cbv2g_tp cbv2g-src)

add_library(cbv2g_iso20 STATIC IMPORTED)
add_library(cbv2g::iso20 ALIAS cbv2g_iso20)
set_property(TARGET cbv2g_iso20 PROPERTY IMPORTED_LOCATION ${CBV2G_LIB_DIR}/libcbv2g_iso20.a)
set_property(TARGET cbv2g_iso20 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CBV2G_INCLUDE_DIR})
add_dependencies(cbv2g_iso20 cbv2g-src)
