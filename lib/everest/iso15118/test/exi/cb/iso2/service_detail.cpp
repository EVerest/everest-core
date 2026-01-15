#include <catch2/catch_test_macros.hpp>

#include <iso15118/message/d2/service_detail.hpp>
#include <iso15118/message/d2/variant.hpp>

#include "helper.hpp"

using namespace iso15118;
namespace dt = d2::msg::data_types;

SCENARIO("Ser/Deserialize d2 service detail messages") {

    GIVEN("Deserialize service detail req") {
        uint8_t doc_raw[] = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x51, 0x91, 0x8C, 0x00};

        const io::StreamInputView stream_view{doc_raw, sizeof(doc_raw)};

        d2::msg::Variant variant(io::v2gtp::PayloadType::SAP, stream_view, false);

        THEN("It should be deserialized successfully") {
            REQUIRE(variant.get_type() == d2::msg::Type::ServiceDetailReq);

            const auto& msg = variant.get<d2::msg::ServiceDetailRequest>();
            const auto& header = msg.header;

            REQUIRE(header.session_id == std::array<uint8_t, 8>{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D});

            REQUIRE(msg.service_id == 99);
        }
    }
    GIVEN("Serialize service detail res") {

        const auto header = d2::msg::Header{{0x02, 0xDB, 0x22, 0x07, 0x3B, 0x08, 0x4D, 0x2D}, std::nullopt};

        auto res = d2::msg::ServiceDetailResponse{};
        res.header = header;
        res.response_code = dt::ResponseCode::OK;
        res.service_id = 99;
        auto parameterSet = dt::ParameterSet{};
        parameterSet.parameter_set_id = 50;

        dt::Parameter boolValue{"bool", true};
        dt::Parameter byteValue{"byte", (int8_t)8};
        dt::Parameter shortValue{"short", (int16_t)16};
        dt::Parameter intValue{"int", (int32_t)32};
        dt::Parameter physicalValue{"physical", dt::PhysicalValue{55, -1, dt::UnitSymbol::A}};
        dt::Parameter stringValue{"string", "Foo Bar"};

        parameterSet.parameter.push_back(boolValue);
        parameterSet.parameter.push_back(byteValue);
        parameterSet.parameter.push_back(shortValue);
        parameterSet.parameter.push_back(intValue);
        parameterSet.parameter.push_back(physicalValue);
        parameterSet.parameter.push_back(stringValue);
        res.service_parameter_list = {parameterSet};

        std::vector<uint8_t> expected = {0x80, 0x98, 0x02, 0x00, 0xB6, 0xC8, 0x81, 0xCE, 0xC2, 0x13, 0x4B, 0x51, 0xA0,
                                         0x01, 0x8C, 0x01, 0x90, 0x06, 0x62, 0x6F, 0x6F, 0x6C, 0x08, 0x01, 0x98, 0x9E,
                                         0x5D, 0x19, 0x4A, 0x20, 0x00, 0xEE, 0x6D, 0x0D, 0xEE, 0x4E, 0x88, 0x10, 0x00,
                                         0x2B, 0x4B, 0x73, 0xA3, 0x08, 0x00, 0x14, 0xE0, 0xD0, 0xF2, 0xE6, 0xD2, 0xC6,
                                         0xC2, 0xD9, 0x04, 0x18, 0x1B, 0x80, 0x10, 0xE6, 0xE8, 0xE4, 0xD2, 0xDC, 0xCF,
                                         0x41, 0x28, 0xCD, 0xED, 0xE4, 0x08, 0x4C, 0x2E, 0x42, 0x80};

        THEN("It should be serialized successfully") {
            REQUIRE(serialize_helper(res) == expected); // FIXME: Failing right now
        }
    }
}
