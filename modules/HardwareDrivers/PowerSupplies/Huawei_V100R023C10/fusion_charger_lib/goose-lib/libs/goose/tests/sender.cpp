// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <algorithm>

#include <goose/sender.hpp>

class DummyEthernetInterface : public goose_ethernet::EthernetInterfaceIntf {
    std::function<void(const uint8_t*, size_t)> send_callback;
    std::function<std::vector<uint8_t>()> receive_callback;

public:
    DummyEthernetInterface(std::function<void(const uint8_t*, size_t)> send_callback,
                           std::function<std::vector<uint8_t>()> receive_callback) :
        send_callback(send_callback), receive_callback(receive_callback) {
    }

    void send_packet_raw(const uint8_t* packet, size_t size) override {
        send_callback(packet, size);
    }

    std::optional<std::vector<uint8_t>> receive_packet_raw() override {
        return receive_callback();
    }

    // Dummy implementation
    const uint8_t* get_mac_address() const override {
        return nullptr;
    }
};

TEST(DummyEthernetInterface, callback_works) {
    uint8_t send_counter = 0;
    uint8_t receive_counter = 0;

    auto dummy_frame = goose_ethernet::EthernetFrame(std::vector<uint8_t>(60));

    DummyEthernetInterface intf(
        [&](const uint8_t* packet, size_t size) {
            ASSERT_EQ(dummy_frame.serialize().size(), size);
            send_counter++;
        },
        [&]() -> std::vector<uint8_t> {
            receive_counter++;
            return dummy_frame.serialize();
        });

    intf.send_packet(dummy_frame);
    EXPECT_EQ(send_counter, 1);

    auto received = intf.receive_packet();
    if (!received.has_value()) {
        FAIL() << "Received frame is empty";
    }
    auto recv = received.value();
    EXPECT_EQ(receive_counter, 1);
    EXPECT_EQ(recv.serialize(), dummy_frame.serialize());
}

TEST(Sender, single_frame_goes_through_all_delays) {
    std::vector<std::chrono::milliseconds> received_times;
    std::vector<uint16_t> received_st_nums;
    std::vector<uint16_t> received_sq_nums;

    auto intf = std::make_shared<DummyEthernetInterface>(
        [&](const uint8_t* data, size_t size) {
            received_times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()));

            auto frame = goose::frame::GooseFrame(goose_ethernet::EthernetFrame(data, size));

            received_sq_nums.push_back(frame.pdu.sq_num);
            received_st_nums.push_back(frame.pdu.st_num);
        },
        []() -> std::vector<uint8_t> { return {}; });

    goose::sender::Sender sender(
        std::chrono::milliseconds(10),
        {std::chrono::milliseconds(1), std::chrono::milliseconds(2), std::chrono::milliseconds(4)}, intf);

    sender.start();

    // Nothing should have been sent yet
    ASSERT_EQ(received_times.size(), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    ASSERT_EQ(received_times.size(), 0);

    goose::frame::GooseFrame frame;
    frame.appid[0] = 0x01;
    frame.appid[1] = 0x02;
    strcpy(frame.pdu.go_cb_ref, "goose_cb_ref");
    strcpy(frame.pdu.dat_set, "dataset");
    strcpy(frame.pdu.go_id, "goose_id");
    frame.pdu.timestamp = goose::frame::GooseTimestamp::now();
    // Random data, should be overwritten by sender
    frame.pdu.sq_num = 0xdead;
    frame.pdu.st_num = 0xbeef;

    sender.send(new goose::sender::SendPacketNormal(frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // this should be about one cycle of all Ts, at least 4 packets should have
    // been sent by now though no more than 5

    ASSERT_GE(received_times.size(), 4);
    ASSERT_LE(received_times.size(), 5);

    // first delay should be about 1ms (+-1ms due to linux not
    // being real-time)s
    ASSERT_LE((received_times[1] - received_times[0]).count(), 2);

    // second delay should be about 2ms (+-1ms)
    ASSERT_NEAR((received_times[2] - received_times[1]).count(), 2, 1);

    // third delay should be about 4ms (+-2ms)
    ASSERT_NEAR((received_times[3] - received_times[2]).count(), 4, 2);

    // fourth delay should be about 10ms (+-4ms) (if a fourth packet was sent)
    if (received_times.size() >= 5) {
        ASSERT_NEAR((received_times[4] - received_times[3]).count(), 10, 4);
    }

    // all st nums should be 1
    for (auto st_num : received_st_nums) {
        ASSERT_EQ(st_num, 1);
    }

    sender.stop();
}

TEST(Sender, multiple_frames_go_through_all_delays) {
    std::vector<std::chrono::milliseconds> received_times;
    std::vector<uint16_t> received_st_nums;
    std::vector<uint16_t> received_sq_nums;

    auto intf = std::make_shared<DummyEthernetInterface>(
        [&](const uint8_t* data, size_t size) {
            received_times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()));

            auto frame = goose::frame::GooseFrame(goose_ethernet::EthernetFrame(data, size));

            received_sq_nums.push_back(frame.pdu.sq_num);
            received_st_nums.push_back(frame.pdu.st_num);
        },
        []() -> std::vector<uint8_t> { return {}; });

    auto t0 = std::chrono::milliseconds(10);
    auto ts = std::vector<std::chrono::milliseconds>{std::chrono::milliseconds(1), std::chrono::milliseconds(2),
                                                     std::chrono::milliseconds(4)};

    goose::sender::Sender sender(t0, ts, intf);

    sender.start();

    // Nothing should have been sent yet
    ASSERT_EQ(received_times.size(), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    ASSERT_EQ(received_times.size(), 0);

    goose::frame::GooseFrame frame;
    frame.appid[0] = 0x01;
    frame.appid[1] = 0x02;
    strcpy(frame.pdu.go_cb_ref, "goose_cb_ref");
    strcpy(frame.pdu.dat_set, "dataset");
    strcpy(frame.pdu.go_id, "goose_id");
    frame.pdu.timestamp = goose::frame::GooseTimestamp::now();
    // Random data, should be overwritten by sender
    frame.pdu.sq_num = 0xdead;
    frame.pdu.st_num = 0xbeef;

    // send multiple frames
    for (int i = 0; i < 5; i++) {
        sender.send(new goose::sender::SendPacketNormal(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    sender.stop();

    auto snapshot_times = received_times;
    auto snapshot_st_nums = received_st_nums;
    auto snapshot_sq_nums = received_sq_nums;

    // sometimes the snapshot_{times,st_nums,sq_nums} are not equal size; thus
    // truncate the longest
    const auto snapshot_sizes = {snapshot_times.size(), snapshot_st_nums.size(), snapshot_sq_nums.size()};
    const auto min_size = *std::min_element(snapshot_sizes.begin(), snapshot_sizes.end());
    snapshot_times.resize(min_size);
    snapshot_st_nums.resize(min_size);
    snapshot_sq_nums.resize(min_size);

    // Assert the predicate that the frames delay increases 3 times, then stays
    // about the same and then st num increases and the frame delay resets and
    // increases again, while the sq num increases by 1 each time and resets upon
    // st num increase
    std::chrono::milliseconds last_delay = std::chrono::milliseconds(0);
    uint16_t last_st_num = 1; // note that the first st num is 1
    uint16_t last_sq_num = 0;
    size_t i_s_since_last_st_num_change = 0;
    size_t st_increase_count = 0;

    for (size_t i = 1; i < snapshot_times.size(); i++) {
        auto delay = snapshot_times[i] - snapshot_times[i - 1];

        // st num should only be equal or increase by 1
        ASSERT_GE(snapshot_st_nums[i], last_st_num);
        ASSERT_LE(snapshot_st_nums[i], last_st_num + 1);

        if (snapshot_st_nums[i] != last_st_num) {
            st_increase_count++;

            // if st num increased, sq num should be 0
            ASSERT_EQ(snapshot_sq_nums[i], 0);

            // delay here is unknown because a message was sent which can be at a
            // random point in time. Though it should not be greater than 20ms
            ASSERT_LE(delay.count(), 20);
        } else {
            // sq num must always get bigger (not in st num change but thats handled
            // above)
            ASSERT_EQ(snapshot_sq_nums[i], last_sq_num + 1);

            // for the first few frames the delay should be near the set delays (ts)
            // (+- 3ms)
            if (i_s_since_last_st_num_change <= 4) {
                // if i_s_since_last_st_num_change is 0 then the last_delay is weird
                // because a message was sent there

                if (i_s_since_last_st_num_change != 0) {
                    // The delay should be near set delays (ts) (+- 3ms)
                    ASSERT_NEAR(delay.count(), ts[i_s_since_last_st_num_change - 1].count(), 3);
                }
            } else {
                // delay should be about t0 (+- 5ms)
                ASSERT_NEAR(delay.count(), t0.count(), 5);
            }
        }

        last_st_num = snapshot_st_nums[i];
        last_sq_num = snapshot_sq_nums[i];
        last_delay = delay;
    }

    ASSERT_EQ(st_increase_count,
              4); // this should be 4 because we send 5 frames, the first
                  // one with st num 1, which is not counted (see the initial
                  // "last_st_num = 1" above)
}
