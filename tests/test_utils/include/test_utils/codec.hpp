#pragma once

#include <cstdint>
#include <memory>

namespace test_utils {

struct EncodingResult {
    bool encoding_successful;
    bool bitstream_match;
};

template <typename DocType>
EncodingResult encode_and_compare(const DocType&, const uint8_t* compare_data, std::size_t length);

template <typename DocType> struct DecodingResult {
    bool decoding_successful;
    DocType value;
};

template <typename DocType> DecodingResult<DocType> decode(const uint8_t* raw_data, std::size_t length);

} // namespace test_utils
