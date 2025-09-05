// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "SerializationTestHelpers.hpp"
#include "everest_api_types/generic/codec.hpp"
#include "nlohmann/json.hpp"
#include <gtest/gtest.h>

using namespace everest::lib::API::V1_0::types::generic;

using namespace everest::lib::API::V1_0::types;

// Warning: these test helpers are generated manually because the serialization of RequestReply works differently
// than other serializations because the to_json function may throw an error
template <>
everest::lib::API::V1_0::types::generic::RequestReply
generate<everest::lib::API::V1_0::types::generic::RequestReply>(bool set_optional_fields) {
    RequestReply generated_object;
    generated_object.replyTo = "mdvChgNGIuAZPRzFEkDOLnsMUcJYyl";
    generated_object.payload = "{\"data\": { \"inner\" : 23, \"inner_str\":\"string\"}}";
    if (set_optional_fields) {
    }
    return generated_object;
}
template <>
void verify<everest::lib::API::V1_0::types::generic::RequestReply>(
    everest::lib::API::V1_0::types::generic::RequestReply original,
    everest::lib::API::V1_0::types::generic::RequestReply result) {
    auto orig_pl = nlohmann::json::parse(original.payload).dump();
    auto res_pl = nlohmann::json::parse(result.payload).dump();

    EXPECT_EQ(original.replyTo, result.replyTo);
    EXPECT_EQ(orig_pl, res_pl);
}

namespace RequestReplyTestHelper {
RequestReply generate(int payload) {
    RequestReply generated_object;
    generated_object.replyTo = "mdvChgNGIuAZPRzFEkDOLnsMUcJYyl";
    generated_object.payload = std::to_string(payload);
    return generated_object;
}
RequestReply generate(std::string payload) {
    RequestReply generated_object;
    generated_object.replyTo = "mdvChgNGIuAZPRzFEkDOLnsMUcJYyl";
    generated_object.payload = payload;
    return generated_object;
}
void test(RequestReply original) {
    auto result = codec_test(original);
    verify(original, result);
}
void test(std::string payload) {
    test(generate(payload));
}
void test(int payload) {
    test(generate(payload));
}
void verify(RequestReply original_object, RequestReply result_object) {
    EXPECT_EQ(original_object.replyTo, result_object.replyTo);
    EXPECT_EQ(original_object.payload, result_object.payload);
}
}; // namespace RequestReplyTestHelper

// Tests
TEST(generic, RequestReply_obj_payload_set) {
    gen_test<everest::lib::API::V1_0::types::generic::RequestReply>();
}
TEST(generic, RequestReply_string_payload_set) {
    EXPECT_THROW({ RequestReplyTestHelper::test("RHwxIpQVSTKyngUNAaOBuEsqCZDbze"); }, std::invalid_argument);
}

TEST(generic, RequestReply_int_payload_set) {
    RequestReplyTestHelper::test(1);
}

TEST(generic, RequestReply_back_and_forth_1) {
    RequestReply rr;
    rr.replyTo = "/this/is/my/reply/address";
    rr.payload = R"( {"data": { "number" : 23, "string": "string", "obj": {"more": 11 }}} )";

    auto ser = serialize(rr);
    auto des = deserialize<RequestReply>(ser);

    EXPECT_EQ(rr.replyTo, des.replyTo);
    EXPECT_EQ(nlohmann::json::parse(rr.payload).dump(), nlohmann::json::parse(des.payload).dump());
}

TEST(generic, RequestReply_back_and_forth_2) {
    RequestReply rr;
    rr.replyTo = "/this/is/my/reply/address";
    rr.payload = "\"helloworld\"";

    auto ser = serialize(rr);
    auto des = deserialize<RequestReply>(ser);
    auto ser2 = serialize(des);
    auto des2 = deserialize<RequestReply>(ser2);

    EXPECT_EQ(rr.replyTo, des.replyTo);
    EXPECT_EQ(rr.payload, des.payload);
    EXPECT_EQ(rr.replyTo, des2.replyTo);
    EXPECT_EQ(rr.payload, des2.payload);
}
