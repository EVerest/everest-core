add_library(cb_exi_codec)

target_sources(cb_exi_codec
    PRIVATE
        common/exi_basetypes_decoder.c
        common/exi_basetypes_encoder.c
        common/exi_basetypes.c
        common/exi_bitstream.c
        common/exi_header.c
        common/exi_types_decoder.c
)

target_include_directories(cb_exi_codec
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(cb_exi_codec PRIVATE c_std_99)

add_library(cb_din)
add_library(cb_v2g::DIN ALIAS cb_din)

target_sources(cb_din
    PRIVATE
        app_handshake/appHand_Datatypes.c
        app_handshake/appHand_Decoder.c
        app_handshake/appHand_Encoder.c
        din/din_msgDefDatatypes.c
        din/din_msgDefDecoder.c
        din/din_msgDefEncoder.c
)

target_include_directories(cb_din
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(cb_din
    PUBLIC
        cb_exi_codec
)

target_compile_features(cb_din PUBLIC c_std_99)

add_library(cb_iso2)
add_library(cb_v2g::ISO2 ALIAS cb_iso2)

target_sources(cb_iso2
    PRIVATE
        app_handshake/appHand_Datatypes.c
        app_handshake/appHand_Decoder.c
        app_handshake/appHand_Encoder.c
        iso_2/iso2_msgDefDatatypes.c
        iso_2/iso2_msgDefDecoder.c
        iso_2/iso2_msgDefEncoder.c
)

target_include_directories(cb_iso2
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(cb_iso2
    PUBLIC
        cb_exi_codec
)

target_compile_features(cb_iso2 PUBLIC c_std_99)

add_library(cb_iso20)
add_library(cb_v2g::ISO20 ALIAS cb_iso20)

target_sources(cb_iso20
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

target_include_directories(cb_iso20
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(cb_iso20
    PUBLIC
        cb_exi_codec
)

target_compile_features(cb_iso20 PUBLIC c_std_99)

add_library(cb_v2gtp)
add_library(cb_v2g::V2GTP ALIAS cb_v2gtp)

target_sources(cb_v2gtp
    PRIVATE
        exi_v2gtp.c
)

target_include_directories(cb_v2gtp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(cb_v2gtp PUBLIC c_std_99)
