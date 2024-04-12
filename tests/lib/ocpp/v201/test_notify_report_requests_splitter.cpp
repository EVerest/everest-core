
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ocpp/v201/notify_report_requests_splitter.hpp>

namespace ocpp {
namespace v201 {

class NotifyReportRequestsSplitterTest : public ::testing::Test {
    int message_count = 0;

protected:
    std::vector<MessageId> message_ids{};
    MessageId generate_message_id() {
        std::stringstream s;
        s << "test_message_" << message_count;
        message_count++;
        message_ids.emplace_back(s.str());
        return message_ids.back();
    }

    // verify returned payloads are actual serializations of Call<NotifyReportRequest> instances
    static void check_valid_call_payload(json payload) {
        ASSERT_EQ(payload.size(), 4);
        ASSERT_THAT(payload[MESSAGE_ID].dump(), testing::MatchesRegex("^\"test_message_[0-9]+\"$"));
        ASSERT_EQ(payload[CALL_ACTION].dump(), R"("NotifyReport")");
        ASSERT_EQ(payload[MESSAGE_TYPE_ID], MessageTypeId::CALL);
        Call<NotifyReportRequest> call{};
        from_json(payload, call);
        ASSERT_EQ(call.msg.get_type(), "NotifyReport");
    }
};

/// \brief Test a request with no report data results into a single message
TEST_F(NotifyReportRequestsSplitterTest, test_create_single_request_no_report_data) {

    // Setup
    NotifyReportRequest req{};
    req.reportData = std::nullopt;
    json req_json = req;
    NotifyReportRequestsSplitter splitter{req, 1000, [this]() { return this->generate_message_id(); }};

    // Act: create payloads
    auto res = splitter.create_call_payloads();

    // Verify: Expect single payload; check fields
    ASSERT_EQ(res.size(), 1);
    auto request = res[0];
    check_valid_call_payload(res[0]);
    ASSERT_EQ("test_message_0", request[1]);
    ASSERT_EQ(json(req_json).dump(), request[3].dump());
}

/// \brief Test a request that fits exactly the provided bound is not split
TEST_F(NotifyReportRequestsSplitterTest, test_create_single_request) {

    // Setup
    NotifyReportRequest req{};
    req.reportData = {ReportData{{"component_name"}, {"variable_name"}, {}, {}, {}},
                      ReportData{{"component_name2"}, {"variable_name2"}, {}, {}, {}}};
    req.requestId = 42;
    req.tbc = false;
    req.seqNo = 0;
    json req_json = req;
    size_t full_size = json{2, "test_message_0", "NotifyReport", req}.dump().size();

    // Create splitter with size exactly fitting the expected payload - thus no split should be done
    NotifyReportRequestsSplitter splitter{req, full_size, [this]() { return this->generate_message_id(); }};
    auto res = splitter.create_call_payloads();

    // Assert no split
    ASSERT_EQ(res.size(), 1);
    auto request = res[0];
    check_valid_call_payload(request);
    ASSERT_EQ(request[1], "test_message_0");

    std::stringstream expected_report_data_json;
    expected_report_data_json << "[" << json(req.reportData.value()[0]).dump() << ","
                              << json(req.reportData.value()[1]).dump() << "]";
    ASSERT_EQ(expected_report_data_json.str(), request[3]["reportData"].dump());
    ASSERT_EQ("false", request[3]["tbc"].dump());
    ASSERT_EQ("0", request[3]["seqNo"].dump());
    ASSERT_EQ(req_json["generatedAt"], request[3]["generatedAt"]);
}

// \brief Test a request that is one byte too long is split
TEST_F(NotifyReportRequestsSplitterTest, test_create_split_request) {
    // Setup
    NotifyReportRequest req{};
    req.requestId = 42;
    req.reportData = {ReportData{{"component_name"}, {"variable_name"}, {}, {}, {}},
                      ReportData{{"component_name2"}, {"variable_name2"}, {}, {}, {}}};
    req.tbc = false;
    json req_json = req;

    size_t full_size = json{2, "test_message_0", "NotifyReport", req}.dump().size();

    // Create splitter with size one less than the expected payload - thus a split should be done
    NotifyReportRequestsSplitter splitter{req, full_size - 1, [this]() { return this->generate_message_id(); }};
    auto res = splitter.create_call_payloads();

    // Verify split is done
    ASSERT_EQ(res.size(), 2);

    for (int i = 0; i < 2; i++) {
        auto request = res[i];
        check_valid_call_payload(request);

        std::stringstream expected_report_data_json;
        expected_report_data_json << "[" << json(req.reportData.value()[i]).dump() << "]";
        ASSERT_EQ(expected_report_data_json.str(), request[3]["reportData"].dump());

        ASSERT_EQ(req_json["generatedAt"], request[3]["generatedAt"]);
        if (i == 0) {
            ASSERT_EQ(request[3]["tbc"].dump(), "true");
            ASSERT_EQ(request[3]["seqNo"].dump(), "0");
        } else {
            ASSERT_EQ(request[3]["tbc"].dump(), "false");
            ASSERT_EQ(request[3]["seqNo"].dump(), "1");
        }
    }
}

//  \brief Test that each split contains at least one report data object, even if it exceeds the size bound.
TEST_F(NotifyReportRequestsSplitterTest, test_splits_contains_at_least_one_report) {
    // Setup
    NotifyReportRequest req{};
    req.requestId = 42;
    req.reportData = {ReportData{{"component_name"}, {"variable_name"}, {}, {}, {}},
                      ReportData{{"component_name2"}, {"variable_name2"}, {}, {}, {}}};
    req.tbc = false;
    json req_json = req;

    // Act: create splitter with minimal max size
    NotifyReportRequestsSplitter splitter{req, 1, [this]() { return this->generate_message_id(); }};
    auto res = splitter.create_call_payloads();

    // Verify message are split into two messages
    ASSERT_EQ(res.size(), 2);
    for (int i = 0; i < 2; i++) {
        auto request = res[i];
        check_valid_call_payload(request);
        ASSERT_EQ(message_ids[i].get(), request[1]);

        std::stringstream expected_report_data_json;
        expected_report_data_json << "[" << json(req.reportData.value()[i]).dump() << "]";
        ASSERT_EQ(expected_report_data_json.str(), request[3]["reportData"].dump());

        ASSERT_EQ(req_json["generatedAt"], request[3]["generatedAt"]);
        if (i == 0) {
            ASSERT_EQ(request[3]["tbc"].dump(), "true");
            ASSERT_EQ(request[3]["seqNo"].dump(), "0");
        } else {
            ASSERT_EQ(request[3]["tbc"].dump(), "false");
            ASSERT_EQ(request[3]["seqNo"].dump(), "1");
        }
    }
}

} // namespace v201
} // namespace ocpp
