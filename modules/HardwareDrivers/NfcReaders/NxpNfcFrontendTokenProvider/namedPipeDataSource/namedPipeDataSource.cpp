// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "namedPipeDataSource.hpp"
#include <algorithm>
#include <cmath>
#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <string_view>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <unistd.h>

namespace {
static bool createdFifo = false;

constexpr mode_t fifo_permissions = 0666;

bool isHexCoded(const std::string& input) {
    return std::all_of(input.begin(), input.end(), [](unsigned char byte) { return std::isxdigit(byte); });
}

void createFifoFile(const std::string& pathToNamedPipe) {
    const std::filesystem::path path = {pathToNamedPipe};
    if (not std::filesystem::is_fifo(path)) {
        if (std::filesystem::exists(path)) {
            throw std::runtime_error("Could not create FIFO at '" + path.string() + "': file exists as non-FIFO");
        }
        // Create FIFO:
        const int ret = mkfifo(pathToNamedPipe.c_str(), fifo_permissions);
        if (ret == -1) {
            throw std::runtime_error("Could not create FIFO at '" + path.string() + "': mkfifo returned '-1'");
        }
        createdFifo = true;
    }
}

class FilePoller {
public:
    explicit FilePoller(const std::string_view& filename) : m_filename(filename) {
        openFileAndConfigureEpoll();
    }

    FilePoller(const FilePoller&) = delete;
    FilePoller(const FilePoller&&) = delete;
    FilePoller operator=(const FilePoller&) = delete;
    FilePoller operator=(const FilePoller&&) = delete;

    ~FilePoller() {
        closeFileAndEpoll();
    }

    std::string wait(int timeout_ms) {
        std::array<struct epoll_event, EVENT_BUFFER_SIZE> m_epoll_event_buffer{};
        const int eventCount = epoll_wait(m_epoll_list_fd, m_epoll_event_buffer.data(), EVENT_BUFFER_SIZE, timeout_ms);
        std::string partialLine;
        if (eventCount > 0) {
            std::array<char, READ_BUFFER_SIZE> buffer{};

            for (int i = 0; i < eventCount; i++) {
                if ((m_epoll_event_buffer.at(i).events & EPOLLIN) != 0) {
                    const struct epoll_event& event = m_epoll_event_buffer.at(i);
                    const std::size_t readCount = read(event.data.fd, buffer.data(), READ_BUFFER_SIZE - 1);
                    buffer.at(readCount) = '\0';
                    partialLine.append(buffer.data());
                }
            }
            // After treating EPOLLIN (input) events, look for EPOLLHUP (Hang-up) events
            // If the exist, reopen file and reconfigure EPoll,
            // otherwise, HUP will happen forever and use the full CPU
            for (int i = 0; i < eventCount; i++) {
                if ((m_epoll_event_buffer.at(i).events & EPOLLHUP) != 0) {
                    reOpenFileAndConfigureEpoll();
                    break;
                }
            }
        }
        return partialLine;
    }

private:
    void openFileAndConfigureEpoll() {
        m_fifo_fd = open(m_filename.c_str(), O_RDONLY | O_NONBLOCK);

        m_epoll_list_fd = epoll_create1(EPOLL_CLOEXEC);
        struct epoll_event m_epoll_event = {};
        m_epoll_event.events = EPOLLIN;
        m_epoll_event.data.fd = m_fifo_fd;

        epoll_ctl(m_epoll_list_fd, EPOLL_CTL_ADD, m_fifo_fd, &m_epoll_event);
    }

    void reOpenFileAndConfigureEpoll() {
        closeFileAndEpoll();
        openFileAndConfigureEpoll();
    }

    void closeFileAndEpoll() const {
        close(m_epoll_list_fd);
        close(m_fifo_fd);
    }

    std::string m_filename;
    int m_fifo_fd{0};
    int m_epoll_list_fd{0};

    static const std::size_t READ_BUFFER_SIZE = 64;
    static const std::size_t EVENT_BUFFER_SIZE = 16;
};

} // namespace

NamedPipeDataSource::NamedPipeDataSource(const std::string& filename) : m_filename(filename) {
    createFifoFile(filename);
}

NamedPipeDataSource::NamedPipeDataSource() :
    NamedPipeDataSource("/tmp/EV_NXP_NFC_FRONTEND_TOKEN_PROVIDER_FIFO_SUBSTITUTE") {
}

NamedPipeDataSource::~NamedPipeDataSource() {
    stopped = true;

    m_line_reader->join();

    if (createdFifo) {
        std::remove(m_filename.c_str());
    }
}

void NamedPipeDataSource::setDetectionCallback(
    const std::function<void(const std::pair<std::string, std::vector<std::uint8_t>>&)>& callback) {
    m_callback = callback;
}

void NamedPipeDataSource::setErrorLogCallback(const std::function<void(const std::string&)>& callback) {
    m_err_callback = callback;
}

void NamedPipeDataSource::run() {
    m_line_reader = std::make_unique<std::thread>(&NamedPipeDataSource::getLinesForever, this);
}

void NamedPipeDataSource::getLinesForever() {
    constexpr int wait_timeout_ms = 1000;
    constexpr std::size_t max_line_length = 128;
    std::string line{};
    FilePoller poller(m_filename);

    while (not stopped) {
        const std::string partialLine = poller.wait(wait_timeout_ms);

        if (not partialLine.empty()) {
            line.append(partialLine);

            const std::size_t cr_pos = line.find('\n');
            if (cr_pos != std::string::npos) {
                if (auto result = parseInput(line)) {
                    m_callback(*result);
                } else {
                    m_err_callback("Could not parse '" + line + "'");
                }

                line.clear();
            } else {
                if (line.size() > max_line_length) {
                    line.clear();
                }
            }
        }
    }
}

std::optional<std::pair<std::string, std::vector<std::uint8_t>>>
NamedPipeDataSource::parseInput(const std::string& input) {
    constexpr std::size_t protocol_designator_len = 8;
    constexpr std::size_t iso14443_uid_len = 8;
    constexpr std::size_t iso15693_uid_len = 16;

    if (input.size() < protocol_designator_len + 1) {
        return std::nullopt;
    }

    const std::string protocol = input.substr(0, protocol_designator_len);

    std::size_t expectedUidSize{0};
    if (protocol == "ISO14443") {
        expectedUidSize = iso14443_uid_len;
    } else if (protocol == "ISO15693") {
        expectedUidSize = iso15693_uid_len;
    } else {
        return std::nullopt;
    }

    // "+2" accounts for the ':' and '\n'
    const std::size_t expectedStringSize = protocol_designator_len + expectedUidSize + 2;

    if (input.size() != expectedStringSize) {
        return std::nullopt;
    }

    const std::string uid_str = input.substr(9, expectedUidSize);
    if (not isHexCoded(uid_str)) {
        return std::nullopt;
    }

    std::vector<std::uint8_t> nfcid;
    for (std::size_t i = 0; i < uid_str.length(); i += 2) {
        const std::string single_byte_str = uid_str.substr(i, 2);
        const std::uint8_t byte = static_cast<std::uint8_t>(std::stoi(single_byte_str, nullptr, 16));
        nfcid.push_back(byte);
    }

    return std::make_pair(protocol, nfcid);
}
