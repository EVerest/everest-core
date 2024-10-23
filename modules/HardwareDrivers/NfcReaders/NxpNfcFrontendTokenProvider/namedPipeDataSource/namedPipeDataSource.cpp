// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "namedPipeDataSource.hpp"
#include <algorithm>
#include <cmath>
#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <unistd.h>

static bool createdFifo = false;

bool isHexCoded(const std::string&);
void createFifoFile(const std::string&);

class FilePoller {
public:
    FilePoller(const std::string& filename) : m_filename(filename) {
        openFileAndConfigureEpoll();
    }

    ~FilePoller() {
        closeFileAndEpoll();
    }

    std::pair<int, std::string> wait(int timeout_ms) {
        struct epoll_event m_epoll_event_buffer[EVENT_BUFFER_SIZE];
        int eventCount = epoll_wait(m_epoll_list_fd, m_epoll_event_buffer, EVENT_BUFFER_SIZE, 1000);
        int ret = eventCount;
        int totalReadCount = 0;
        std::string partialLine;
        if (eventCount > 0) {
            totalReadCount = 0;
            char buffer[READ_BUFFER_SIZE];
            for (int i = 0; i < eventCount; i++) {
                if (m_epoll_event_buffer[i].events & EPOLLIN) {
                    struct epoll_event& event = m_epoll_event_buffer[i];
                    int readCount = read(event.data.fd, buffer, READ_BUFFER_SIZE - 1);
                    buffer[readCount] = '\0';
                    totalReadCount += readCount;
                    partialLine.append(buffer);
                }
            }
            // After treating EPOLLIN (input) events, look for EPOLLHUP (Hang-up) events
            // If the exist, reopen file and reconfigure EPoll,
            // otherwise, HUP will happen forever and use the full CPU
            for (int i = 0; i < eventCount; i++) {
                if (m_epoll_event_buffer[i].events & EPOLLHUP) {
                    reOpenFileAndConfigureEpoll();
                    break;
                }
            }
        }
        return std::pair<int, std::string>(totalReadCount, partialLine);
    }

private:
    void openFileAndConfigureEpoll() {
        m_fifo_fd = open(m_filename.c_str(), O_RDONLY | O_NONBLOCK);

        m_epoll_list_fd = epoll_create1(0);
        struct epoll_event m_epoll_event;
        m_epoll_event.events = EPOLLIN;
        m_epoll_event.data.fd = m_fifo_fd;

        epoll_ctl(m_epoll_list_fd, EPOLL_CTL_ADD, m_fifo_fd, &m_epoll_event);
    }

    void reOpenFileAndConfigureEpoll() {
        closeFileAndEpoll();
        openFileAndConfigureEpoll();
    }

    void closeFileAndEpoll() {
        close(m_epoll_list_fd);
        close(m_fifo_fd);
    }

    std::string m_filename;
    int m_fifo_fd;
    int m_epoll_list_fd;

    static const std::size_t READ_BUFFER_SIZE = 64;
    static const size_t EVENT_BUFFER_SIZE = 16;
};

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
    std::string line;
    FilePoller poller(m_filename);

    while (not stopped) {
        auto [byteCount, partialLine] = poller.wait(1000);
        if (byteCount == -1) {
            break;
        }

        if (partialLine.size() > 0) {
            line.append(partialLine);

            size_t cr_pos = line.find('\n');
            if (cr_pos != std::string::npos) {
                if (auto result = parseInput(line)) {
                    m_callback(*result);
                } else {
                    m_err_callback("Could not parse '" + line + "'");
                }

                line.clear();
            } else {
                if (line.size() > 128) {
                    line.clear();
                }
            }
        }
    }
}

std::optional<std::pair<std::string, std::vector<std::uint8_t>>> NamedPipeDataSource::parseInput(std::string input) {
    input.erase(input.find('\n'));

    std::size_t expectedStringSize = 8;
    std::size_t expectedUidSize = 0;
    std::string protocol;

    if (input.size() < expectedStringSize) {
        return std::nullopt;
    }
    std::string proto_str = input.substr(0, 8);
    if (proto_str == "ISO14443") {
        protocol = proto_str;
        expectedUidSize = 4 * 2;
        expectedStringSize = 17;
    } else if (proto_str == "ISO15693") {
        protocol = proto_str;
        expectedUidSize = 8 * 2;
        expectedStringSize = 25;
    } else {
        return std::nullopt;
    }
    if (input.size() != expectedStringSize) {
        return std::nullopt;
    }
    std::string uid_str = input.substr(9, expectedUidSize);
    if (not isHexCoded(uid_str)) {
        return std::nullopt;
    }

    std::vector<std::uint8_t> nfcid;
    for (size_t i = 0; i < uid_str.length(); i += 2) {
        std::string single_byte_str = uid_str.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(single_byte_str, nullptr, 16));
        nfcid.push_back(byte);
    }

    return std::make_pair(protocol, nfcid);
}

bool isHexCoded(const std::string& input) {
    return std::all_of(input.begin(), input.end(), [](unsigned char c) { return std::isxdigit(c); });
}

void createFifoFile(const std::string& pathToNamedPipe) {
    std::filesystem::path path = {pathToNamedPipe};
    if (not std::filesystem::is_fifo(path)) {
        if (std::filesystem::exists(path)) {
            throw std::runtime_error("Could not create FIFO at '" + path.string() + "': file exists as non-FIFO");
        }
        // Create FIFO:
        int ret = mkfifo(pathToNamedPipe.c_str(), 0666);
        if (ret == -1) {
            throw std::runtime_error("Could not create FIFO at '" + path.string() + "': mkfifo returned '-1'");
        }
        createdFifo = true;
    }
}
