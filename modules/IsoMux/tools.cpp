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

unsigned int generate_srand_seed(void) {
    unsigned int s;

    if (generate_random_data(&s, sizeof(s)) == -1)
        return 42; /* just to _not_ use 1 which is the default value when srand is not used at all */

    return s;
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

/*
 * lhs < rhs:  return < 0
 * lhs == rhs: return 0
 * lhs > rhs:  return > 0
 */
int timespec_compare(const struct timespec* lhs, const struct timespec* rhs) {
    if (lhs->tv_sec < rhs->tv_sec)
        return -1;
    if (lhs->tv_sec > rhs->tv_sec)
        return 1;
    return lhs->tv_nsec - rhs->tv_nsec;
}

long long timespec_to_ms(struct timespec ts) {
    return ((long long)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

long long timespec_to_us(struct timespec ts) {
    return ((long long)ts.tv_sec * 1000000) + (ts.tv_nsec / 1000);
}

int msleep(int ms) {
    struct timespec req, rem;

    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * (1000 * 1000); /* x ms */

    while ((nanosleep(&req, &rem) == (-1)) && (errno == EINTR)) {
        req = rem;
    }

    return 0;
}

long long int getmonotonictime() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

double calc_physical_value(const int16_t& value, const int8_t& multiplier) {
    return static_cast<double>(value * pow(10.0, multiplier));
}

bool range_check_int32(int32_t min, int32_t max, int32_t value) {
    return ((value < min) || (value > max)) ? false : true;
}

bool range_check_int64(int64_t min, int64_t max, int64_t value) {
    return ((value < min) || (value > max)) ? false : true;
}

void round_down(const char* buffer, size_t len) {
    char* p;

    p = (char*)strchr(buffer, '.');

    if (!p)
        return;

    if (p - buffer > len - 2)
        return;

    if (*(p + 1) == '\0')
        return;

    *(p + 2) = '\0';
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

uint8_t get_dir_numbered_file_names(char file_names[MAX_PKI_CA_LENGTH][MAX_FILE_NAME_LENGTH], const char* path,
                                    const char* prefix, const char* suffix, const uint8_t offset,
                                    const uint8_t max_idx) {
    if (path == NULL) {
        dlog(DLOG_LEVEL_ERROR, "Invalid file path");
        return 0;
    }

    DIR* d = opendir(path); // open the path

    if (d == NULL) {
        dlog(DLOG_LEVEL_ERROR, "Unable to open file path %s", path);
        return 0;
    }
    struct dirent* dir; // for the directory entries
    uint8_t num_of_files = 0;
    uint8_t prefix_len = std::string(prefix).size();
    uint8_t suffix_len = std::string(suffix).size();
    uint8_t min_idx = max_idx; // helper value to re-sort array

    while (((dir = readdir(d)) != NULL) && (num_of_files != (max_idx - offset))) {
        if (dir->d_type != DT_DIR) {
            /* if the type is not directory*/
            if ((std::string(dir->d_name).size() > (prefix_len + suffix_len + 1)) && /* Plus one for the numbering */
                (0 == strncmp(prefix, dir->d_name, prefix_len))) {
                for (uint8_t idx = offset; idx < max_idx; idx++) {
                    /* Iterated over the number prefix */
                    if ((dir->d_name[prefix_len] == ('0' + idx)) &&
                        (strncmp(suffix, &dir->d_name[prefix_len + 1], suffix_len) == 0)) {
                        if (MAX_FILE_NAME_LENGTH >= std::string(dir->d_name).size()) {
                            strcpy(file_names[idx], dir->d_name);
                            // dlog(DLOG_LEVEL_ERROR,"Cert-file found: %s", &AFileNames[idx]);
                            num_of_files++;
                            min_idx = std::min(min_idx, idx);
                        } else {
                            dlog(DLOG_LEVEL_ERROR, "Max. file-name size exceeded. Only %i characters supported",
                                 MAX_FILE_NAME_LENGTH);
                            num_of_files = 0;
                            goto exit;
                        }
                    }
                }
            }
        } else if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            /*if it is a directory*/
        }
    }
    /* Re-sort array. This part fills gaps in the array. For example if there is only one file with
     * number _4_, the following code will cpy the file name of index 3 of the AFileNames array to
     * index 0 (In case AOffset is set to 0) */
    if (min_idx != offset) {
        for (uint8_t idx = offset; (idx - offset) < num_of_files; idx++) {
            strcpy(file_names[idx], file_names[min_idx + idx - offset]);
        }
    }

exit:
    closedir(d);

    return num_of_files + offset;
}

std::string convert_to_hex_str(const uint8_t* data, int len) {
    std::stringstream string_stream;
    string_stream << std::hex;

    for (int idx = 0; idx < len; ++idx)
        string_stream << std::setw(2) << std::setfill('0') << (int)data[idx];

    return string_stream.str();
}

types::iso15118_charger::HashAlgorithm
convert_to_hash_algorithm(const types::evse_security::HashAlgorithm hash_algorithm) {
    switch (hash_algorithm) {
    case types::evse_security::HashAlgorithm::SHA256:
        return types::iso15118_charger::HashAlgorithm::SHA256;
    case types::evse_security::HashAlgorithm::SHA384:
        return types::iso15118_charger::HashAlgorithm::SHA384;
    case types::evse_security::HashAlgorithm::SHA512:
        return types::iso15118_charger::HashAlgorithm::SHA512;
    default:
        throw std::runtime_error(
            "Could not convert types::evse_security::HashAlgorithm to types::iso15118_charger::HashAlgorithm");
    }
}

std::vector<types::iso15118_charger::CertificateHashDataInfo>
convert_to_certificate_hash_data_info_vector(const types::evse_security::OCSPRequestDataList& ocsp_request_data_list) {
    std::vector<types::iso15118_charger::CertificateHashDataInfo> certificate_hash_data_info_vec;
    for (const auto& ocsp_request_data : ocsp_request_data_list.ocsp_request_data_list) {
        if (ocsp_request_data.responder_url.has_value() and ocsp_request_data.certificate_hash_data.has_value()) {
            types::iso15118_charger::CertificateHashDataInfo certificate_hash_data;
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
