// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <charge_bridge/utilities/filesystem.hpp>
#include <charge_bridge/utilities/sync_udp_client.hpp>

#include <fstream>

namespace charge_bridge::firmware_update {

struct fw_update_config {
    std::string cb;
    std::uint16_t cb_port;
    std::string cb_remote;
    std::string fw_path;
    bool fw_update_on_start;
};

class sync_fw_updater {
public:
    sync_fw_updater(fw_update_config const& config);
    ~sync_fw_updater() = default;

    std::optional<std::string> get_fw_version();
    bool switch_bank();
    bool ping();
    bool upload_fw();

    void print_fw_version();
    bool print_switch_bank();
    bool quick_check_connection();
    bool check_connection();
    bool check_if_correct_fw_installed();

private:
    bool check_reply(utilities::sync_udp_client::reply const& val);

    bool upload_firmware();

    bool upload_init(const fs::path& file_path, std::uint32_t& offset,
                     charge_bridge::filesystem_utils::CryptSignedHeader& hdr);
    bool upload_transfer(const fs::path& file_path, std::uint16_t& sector, std::uint32_t offset,
                         std::uint32_t& total_bytes);
    bool upload_finish(const fs::path& file_path, std::uint32_t total_bytes,
                       const charge_bridge::filesystem_utils::CryptSignedHeader& hdr);

    everest::lib::io::udp::udp_payload make_fw_chunk(std::uint16_t sector, std::uint8_t last_chunk,
                                                     std::vector<std::uint8_t> const& data);

    utilities::sync_udp_client m_udp;
    fw_update_config m_config;
    static const std::uint32_t app_udp_sector_size;
    static const std::uint16_t sub_chunk_size;
};
} // namespace charge_bridge::firmware_update
