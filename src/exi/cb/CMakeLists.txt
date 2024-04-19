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
        ${PROJECT_SOURCE_DIR}/include/exi/cb
)

target_compile_features(cb_exi_codec PRIVATE c_std_99)

add_library(cb_iso20)
add_library(iso15118::exi_20 ALIAS cb_iso20)


target_sources(cb_iso20
    PRIVATE
        appHandshake/appHand_Datatypes.c
        appHandshake/appHand_Decoder.c
        appHandshake/appHand_Encoder.c
        iso-20/iso20_AC_Datatypes.c
        iso-20/iso20_AC_Decoder.c 
        iso-20/iso20_AC_Encoder.c 
        iso-20/iso20_CommonMessages_Datatypes.c 
        iso-20/iso20_CommonMessages_Decoder.c  
        iso-20/iso20_CommonMessages_Encoder.c  
        iso-20/iso20_DC_Datatypes.c  
        iso-20/iso20_DC_Decoder.c 
        iso-20/iso20_DC_Encoder.c
)

target_include_directories(cb_iso20
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/exi/cb
)

target_link_libraries(cb_iso20
    PRIVATE
        cb_exi_codec
)

target_compile_features(cb_iso20 PRIVATE c_std_99)

add_library(cb_v2gtp)
add_library(iso15118::v2gtp ALIAS cb_v2gtp)

target_sources(cb_v2gtp
    PRIVATE
        v2gtp/exi_v2gtp.c
)

target_include_directories(cb_v2gtp
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/exi/cb
)

target_compile_features(cb_v2gtp PRIVATE c_std_99)
