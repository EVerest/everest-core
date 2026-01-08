// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "tools.hpp"
#include "log.hpp"
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

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs) {
    struct timespec ts_delta;

    set_normalized_timespec(&ts_delta, lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);

    return ts_delta;
}

long long timespec_to_ms(struct timespec ts) {
    return ((long long)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

bool get_dir_filename(char* file_name, uint8_t file_name_len, const char* path, const char* file_name_identifier) {

    file_name[0] = '\0';

    if (path == NULL) {
        dlog(DLOG_LEVEL_ERROR, "Invalid file path");
        return false;
    }
    DIR* d = opendir(path); // open the path

    if (d == NULL) {
        dlog(DLOG_LEVEL_ERROR, "Unable to open file path %s", path);
        return false;
    }
    struct dirent* dir; // for the directory entries
    uint8_t file_name_identifier_len = std::string(file_name_identifier).size();
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type != DT_DIR) {
            /* if the type is not directory*/
            if ((std::string(dir->d_name).size() > (file_name_identifier_len)) && /* Plus one for the numbering */
                (strncmp(file_name_identifier, dir->d_name, file_name_identifier_len) == 0) &&
                (file_name_len > std::string(dir->d_name).size())) {
                strncpy(file_name, dir->d_name, std::string(dir->d_name).size() + 1);
                break;
            }
        }
    }

    closedir(d);

    return (file_name[0] != '\0');
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
