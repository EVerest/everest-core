// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef NAMED_PIPE_TOKEN_PROVIDER_IMPL_HPP
#define NAMED_PIPE_TOKEN_PROVIDER_IMPL_HPP

#include <atomic>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

class NamedPipeDataSource {
public:
    NamedPipeDataSource();
    explicit NamedPipeDataSource(const std::string& filename);
    NamedPipeDataSource(const NamedPipeDataSource&) = delete;
    NamedPipeDataSource(const NamedPipeDataSource&&) = delete;
    NamedPipeDataSource operator=(const NamedPipeDataSource&) = delete;
    NamedPipeDataSource operator=(const NamedPipeDataSource&&) = delete;
    ~NamedPipeDataSource();

    void setDetectionCallback(const std::function<void(const std::pair<std::string, std::vector<std::uint8_t>>&)>&);
    void setErrorLogCallback(const std::function<void(const std::string&)>&);
    void run();

private:
    void getLinesForever();
    std::optional<std::pair<std::string, std::vector<std::uint8_t>>> parseInput(const std::string& input);

    std::string m_filename;

    std::function<void(const std::pair<std::string, std::vector<std::uint8_t>>&)> m_callback;
    std::function<void(const std::string&)> m_err_callback;

    std::unique_ptr<std::thread> m_line_reader;
    std::atomic<bool> stopped{false};
};

#endif // NAMED_PIPE_TOKEN_PROVIDER_IMPL_HPP
