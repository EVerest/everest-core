// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "tools.hpp"
#include "log.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <iomanip>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <array>

ssize_t safe_read(int fd, void* buf, size_t count) {
    for (;;) {
        ssize_t result = read(fd, buf, count);

        if (result >= 0)
            return result;
        else if (errno == EINTR)
            continue;
        else
            return result;
    }
}

int generate_random_data(void* dest, size_t dest_len) {
    size_t len = 0;
    int fd;

    fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1)
        return -1;

    while (len < dest_len) {
        ssize_t rv = safe_read(fd, dest, dest_len);

        if (rv < 0) {
            close(fd);
            return -1;
        }

        len += rv;
    }

    close(fd);
    return 0;
}

const char* choose_first_ipv6_interface() {
    struct ifaddrs *ifaddr, *ifa;
    char buffer[INET6_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1)
        return NULL;

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET6) {
            inet_ntop(AF_INET6, &ifa->ifa_addr->sa_data, buffer, sizeof(buffer));
            if (strstr(buffer, "fe80") != NULL) {
                return ifa->ifa_name;
            }
        }
    }
    dlog(DLOG_LEVEL_ERROR, "No necessary IPv6 link-local address was found!");
    return NULL;
}

int get_interface_ipv6_address(const char* if_name, enum Addr6Type type, struct sockaddr_in6* addr) {
    struct ifaddrs *ifaddr, *ifa;
    int rv = -1;

    // If using loopback device, accept any address
    // (lo usually does not have a link local address)
    if (strcmp(if_name, "lo") == 0) {
        type = ADDR6_TYPE_UNPSEC;
    }

    if (getifaddrs(&ifaddr) == -1)
        return -1;

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family != AF_INET6)
            continue;

        if (strcmp(ifa->ifa_name, if_name) != 0)
            continue;

        /* on Linux the scope_id is interface index for link-local addresses */
        switch (type) {
        case ADDR6_TYPE_GLOBAL: /* no link-local address requested */
            if ((reinterpret_cast<struct sockaddr_in6*>(ifa->ifa_addr))->sin6_scope_id != 0)
                continue;
            break;

        case ADDR6_TYPE_LINKLOCAL: /* link-local address requested */
            if ((reinterpret_cast<struct sockaddr_in6*>(ifa->ifa_addr))->sin6_scope_id == 0)
                continue;
            break;

        default: /* any address of the interface requested */
            /* use first found */
            break;
        }

        memcpy(addr, ifa->ifa_addr, sizeof(*addr));

        rv = 0;
        goto out;
    }

out:
    freeifaddrs(ifaddr);
    return rv;
}

#define NSEC_PER_SEC 1000000000L

void set_normalized_timespec(struct timespec* ts, time_t sec, int64_t nsec) {
    while (nsec >= NSEC_PER_SEC) {
        nsec -= NSEC_PER_SEC;
        ++sec;
    }
    while (nsec < 0) {
        nsec += NSEC_PER_SEC;
        --sec;
    }
    ts->tv_sec = sec;
    ts->tv_nsec = nsec;
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs) {
    struct timespec ts_delta;

    set_normalized_timespec(&ts_delta, lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);

    return ts_delta;
}

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs) {
    struct timespec ts_delta;

    set_normalized_timespec(&ts_delta, lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);

    return ts_delta;
}

void timespec_add_ms(struct timespec* ts, long long msec) {
    long long sec = msec / 1000;

    set_normalized_timespec(ts, ts->tv_sec + sec, ts->tv_nsec + (msec - sec * 1000) * 1000 * 1000);
}

long long timespec_to_ms(struct timespec ts) {
    return ((long long)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

long long int getmonotonictime() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

double calc_physical_value(const int16_t& value, const int8_t& multiplier) {
    return static_cast<double>(value * pow(10.0, multiplier));
}

types::iso15118::HashAlgorithm convert_to_hash_algorithm(const types::evse_security::HashAlgorithm hash_algorithm) {
    switch (hash_algorithm) {
    case types::evse_security::HashAlgorithm::SHA256:
        return types::iso15118::HashAlgorithm::SHA256;
    case types::evse_security::HashAlgorithm::SHA384:
        return types::iso15118::HashAlgorithm::SHA384;
    case types::evse_security::HashAlgorithm::SHA512:
        return types::iso15118::HashAlgorithm::SHA512;
    default:
        throw std::runtime_error(
            "Could not convert types::evse_security::HashAlgorithm to types::iso15118::HashAlgorithm");
    }
}

std::vector<types::iso15118::CertificateHashDataInfo>
convert_to_certificate_hash_data_info_vector(const types::evse_security::OCSPRequestDataList& ocsp_request_data_list) {
    std::vector<types::iso15118::CertificateHashDataInfo> certificate_hash_data_info_vec;
    for (const auto& ocsp_request_data : ocsp_request_data_list.ocsp_request_data_list) {
        if (ocsp_request_data.responder_url.has_value() and ocsp_request_data.certificate_hash_data.has_value()) {
            types::iso15118::CertificateHashDataInfo certificate_hash_data;
            certificate_hash_data.hashAlgorithm =
                convert_to_hash_algorithm(ocsp_request_data.certificate_hash_data.value().hash_algorithm);
            certificate_hash_data.issuerNameHash = ocsp_request_data.certificate_hash_data.value().issuer_name_hash;
            certificate_hash_data.issuerKeyHash = ocsp_request_data.certificate_hash_data.value().issuer_key_hash;
            certificate_hash_data.serialNumber = ocsp_request_data.certificate_hash_data.value().serial_number;
            certificate_hash_data.responderURL = ocsp_request_data.responder_url.value();
            certificate_hash_data_info_vec.push_back(certificate_hash_data);
        }
    }
    return certificate_hash_data_info_vec;
}

std::string to_mac_address_str(const uint8_t* const ptr, size_t len) {
    std::string result;
    if ((ptr != nullptr) && (len > 0) && (len <= 16)) {
        for (uint8_t i = 0; i < len; i++) {
            std::array<char, 4> buffer{};
            const auto res = snprintf(&buffer[0], sizeof(buffer), "%02X:", ptr[i]);
            if (res == 3) {
                result += std::string{buffer.data(), 3};
            }
        }
        if (!result.empty()) {
            result.pop_back();
        }
    }
    return result;
}

void strncpy_to_v2g(char* characters, size_t size_of_characters, uint16_t* characters_len, const std::string& src) {
    // copy at max the size of the destination array but reserve one byte for trailing NUL byte;
    // this should not be needed, but just in case someone wants to access the field again
    // using standard string functions, then we are safe
    auto len = std::min(src.length(), size_of_characters - 1);

    memcpy(characters, src.c_str(), len);
    characters[len] = '\0';

    *characters_len = len;
}
