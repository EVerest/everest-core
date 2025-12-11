add_library(cbv2g_exi_codec)
add_library(cbv2g::exi_codec ALIAS cbv2g_exi_codec)
set_property(TARGET cbv2g_exi_codec PROPERTY EXPORT_NAME exi_codec)

target_sources(cbv2g_exi_codec
    PRIVATE
        common/exi_basetypes_decoder.c
        common/exi_basetypes_encoder.c
        common/exi_basetypes.c
        common/exi_bitstream.c
        common/exi_header.c
        common/exi_types_decoder.c
)

target_include_directories(cbv2g_exi_codec
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(cbv2g_exi_codec PRIVATE c_std_99)

add_library(cbv2g_din)
add_library(cbv2g::din ALIAS cbv2g_din)
set_property(TARGET cbv2g_din PROPERTY EXPORT_NAME din)

target_sources(cbv2g_din
    PRIVATE
        app_handshake/appHand_Datatypes.c
        app_handshake/appHand_Decoder.c
        app_handshake/appHand_Encoder.c
        din/din_msgDefDatatypes.c
        din/din_msgDefDecoder.c
        din/din_msgDefEncoder.c
)

target_include_directories(cbv2g_din
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(cbv2g_din
    PUBLIC
        cbv2g::exi_codec
)

target_compile_features(cbv2g_din PUBLIC c_std_99)

add_library(cbv2g_iso2)
add_library(cbv2g::iso2 ALIAS cbv2g_iso2)
set_property(TARGET cbv2g_iso2 PROPERTY EXPORT_NAME iso2)

target_sources(cbv2g_iso2
    PRIVATE
        app_handshake/appHand_Datatypes.c
        app_handshake/appHand_Decoder.c
        app_handshake/appHand_Encoder.c
        iso_2/iso2_msgDefDatatypes.c
        iso_2/iso2_msgDefDecoder.c
        iso_2/iso2_msgDefEncoder.c
)

target_include_directories(cbv2g_iso2
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(cbv2g_iso2
    PUBLIC
        cbv2g::exi_codec
)

target_compile_features(cbv2g_iso2 PUBLIC c_std_99)

add_library(cbv2g_iso20)
add_library(cbv2g::iso20 ALIAS cbv2g_iso20)
set_property(TARGET cbv2g_iso20 PROPERTY EXPORT_NAME iso20)

target_sources(cbv2g_iso20
    PRIVATE
        app_handshake/appHand_Datatypes.c
        app_handshake/appHand_Decoder.c
        app_handshake/appHand_Encoder.c
        iso_20/iso20_AC_Datatypes.c
        iso_20/iso20_AC_Decoder.c
        iso_20/iso20_AC_Encoder.c
        iso_20/iso20_ACDP_Datatypes.c
        iso_20/iso20_ACDP_Decoder.c
        iso_20/iso20_ACDP_Encoder.c
        iso_20/iso20_CommonMessages_Datatypes.c
        iso_20/iso20_CommonMessages_Decoder.c
        iso_20/iso20_CommonMessages_Encoder.c
        iso_20/iso20_DC_Datatypes.c
        iso_20/iso20_DC_Decoder.c
        iso_20/iso20_DC_Encoder.c
        iso_20/iso20_WPT_Datatypes.c
        iso_20/iso20_WPT_Decoder.c
        iso_20/iso20_WPT_Encoder.c
)

target_include_directories(cbv2g_iso20
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(cbv2g_iso20
    PUBLIC
        cbv2g::exi_codec
)

target_compile_features(cbv2g_iso20 PUBLIC c_std_99)

add_library(cbv2g_tp)
add_library(cbv2g::tp ALIAS cbv2g_tp)
set_property(TARGET cbv2g_tp PROPERTY EXPORT_NAME tp)


target_sources(cbv2g_tp
    PRIVATE
        exi_v2gtp.c
)

target_include_directories(cbv2g_tp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(cbv2g_tp PUBLIC c_std_99)
