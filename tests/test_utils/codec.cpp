#include "test_utils/codec.hpp"

#include <vector>

#include <cbv2g/app_handshake/appHand_Decoder.h>
#include <cbv2g/app_handshake/appHand_Encoder.h>

#include <cbv2g/din/din_msgDefDecoder.h>
#include <cbv2g/din/din_msgDefEncoder.h>

#include <cbv2g/iso_20/iso20_AC_Decoder.h>
#include <cbv2g/iso_20/iso20_AC_Encoder.h>
#include <cbv2g/iso_20/iso20_DC_Decoder.h>
#include <cbv2g/iso_20/iso20_DC_Encoder.h>

namespace test_utils {

template <typename DocType>
static EncodingResult encode(int (*encode_func)(exi_bitstream_t*, DocType*), const DocType& request,
                             const uint8_t* compare_data, std::size_t length) {
    // FIXME (aw): what general size to take here?
    uint8_t stream[256] = {};
    exi_bitstream_t exi_stream_in;
    size_t pos1 = 0;

    exi_bitstream_init(&exi_stream_in, stream, sizeof(stream), pos1, nullptr);

    if (0 != encode_func(&exi_stream_in, const_cast<DocType*>(&request))) {
        return {false, false};
    }

    const auto encoded_stream = std::vector<uint8_t>(stream, stream + exi_bitstream_get_length(&exi_stream_in));

    const auto expected_exi_stream = std::vector<uint8_t>(compare_data, compare_data + length);

    return {true, encoded_stream == expected_exi_stream};
}

template <typename DocType>
DecodingResult<DocType> decode(int (*decode_func)(exi_bitstream_t*, DocType*), const uint8_t* raw_data,
                               std::size_t length) {
    exi_bitstream_t exi_stream_in;
    size_t pos1 = 0;

    exi_bitstream_init(&exi_stream_in, const_cast<uint8_t*>(raw_data), length, pos1, nullptr);

    DecodingResult<DocType> result;
    result.decoding_successful = (decode_func(&exi_stream_in, &result.value) == 0);
    return result;
}

//
// app handshake
//
template <>
EncodingResult encode_and_compare(const appHand_exiDocument& request, const uint8_t* compare_data, std::size_t length) {
    return encode(&encode_appHand_exiDocument, request, compare_data, length);
}

template <> DecodingResult<appHand_exiDocument> decode(const uint8_t* raw_data, std::size_t length) {
    return decode(&decode_appHand_exiDocument, raw_data, length);
}

//
// din
//
template <>
EncodingResult encode_and_compare(const din_exiDocument& request, const uint8_t* compare_data, std::size_t length) {
    return encode(&encode_din_exiDocument, request, compare_data, length);
}

template <> DecodingResult<din_exiDocument> decode(const uint8_t* raw_data, std::size_t length) {
    return decode(&decode_din_exiDocument, raw_data, length);
}

//
// iso20 ac
//
template <>
EncodingResult encode_and_compare(const iso20_ac_exiDocument& request, const uint8_t* compare_data,
                                  std::size_t length) {
    return encode(&encode_iso20_ac_exiDocument, request, compare_data, length);
}

template <> DecodingResult<iso20_ac_exiDocument> decode(const uint8_t* raw_data, std::size_t length) {
    return decode(&decode_iso20_ac_exiDocument, raw_data, length);
}

//
// iso20 dc
//
template <>
EncodingResult encode_and_compare(const iso20_dc_exiDocument& request, const uint8_t* compare_data,
                                  std::size_t length) {
    return encode(&encode_iso20_dc_exiDocument, request, compare_data, length);
}

template <> DecodingResult<iso20_dc_exiDocument> decode(const uint8_t* raw_data, std::size_t length) {
    return decode(&decode_iso20_dc_exiDocument, raw_data, length);
}

} // namespace test_utils
