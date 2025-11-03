// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/detail/cb_exi.hpp>

exi_bitstream_t get_exi_input_stream(const iso15118::io::StreamInputView& buffer_view) {
    exi_bitstream_t exi_stream_in;
    exi_bitstream_init(&exi_stream_in, const_cast<uint8_t*>(buffer_view.payload), buffer_view.payload_len, 0, nullptr);

    return exi_stream_in;
}

exi_bitstream_t get_exi_output_stream(const iso15118::io::StreamOutputView& buffer_view) {
    exi_bitstream_t exi_stream_out;
    exi_bitstream_init(&exi_stream_out, buffer_view.payload, buffer_view.payload_len, 0, nullptr);

    return exi_stream_out;
}
