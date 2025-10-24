// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#ifndef TOOLS_H
#define TOOLS_H

#include <generated/types/evse_security.hpp>
#include <generated/types/iso15118.hpp>
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <vector>

#define MAX_FILE_NAME_LENGTH 100
#define MAX_PKI_CA_LENGTH    4 /* leaf up to root certificate */

#ifndef ROUND_UP
#define ROUND_UP(N, S) ((((N) + (S)-1) / (S)) * (S))
#endif

#ifndef ROUND_UP_ELEMENTS
#define ROUND_UP_ELEMENTS(N, S) (((N) + (S)-1) / (S))
#endif

int generate_random_data(void* dest, size_t dest_len);

enum Addr6Type {
    ADDR6_TYPE_UNPSEC = -1,
    ADDR6_TYPE_GLOBAL = 0,
    ADDR6_TYPE_LINKLOCAL = 1,
};

const char* choose_first_ipv6_interface();
int get_interface_ipv6_address(const char* if_name, enum Addr6Type type, struct sockaddr_in6* addr);

void set_normalized_timespec(struct timespec* ts, time_t sec, int64_t nsec);
struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);
struct timespec timespec_add(struct timespec lhs, struct timespec rhs);
void timespec_add_ms(struct timespec* ts, long long msec);
long long timespec_to_ms(struct timespec ts);
long long int getmonotonictime(void);

/*!
 *  \brief calc_physical_value This function calculates the physical value consists on a value and multiplier.
 *  \param value is the value of the physical value
 *  \param multiplier is the multiplier of the physical value
 *  \return Returns the physical value
 */
double calc_physical_value(const int16_t& value, const int8_t& multiplier);

/**
 * \brief convert the given \p hash_algorithm to type types::iso15118::HashAlgorithm
 * \param hash_algorithm
 * \return types::iso15118::HashAlgorithm
 */
types::iso15118::HashAlgorithm convert_to_hash_algorithm(const types::evse_security::HashAlgorithm hash_algorithm);

/**
 * \brief convert the given \p ocsp_request_data_list to std::vector<types::iso15118::CertificateHashDataInfo>
 * \param ocsp_request_data_list
 * \return std::vector<types::iso15118::CertificateHashDataInfo>
 */
std::vector<types::iso15118::CertificateHashDataInfo>
convert_to_certificate_hash_data_info_vector(const types::evse_security::OCSPRequestDataList& ocsp_request_data_list);
/**
 * \brief Convert a buffer of bytes into a MAC-style string.
 *
 * Produces a colon-separated, uppercase hexadecimal string (e.g. "01:23:45:67:89:AB").
 *
 * Formatting rules:
 *  - Each byte in \p data (up to \p datalen) is printed as two uppercase hex digits.
 *  - If \p datalen >= 6, all bytes are printed (output may exceed 6 groups).
 *  - If \p datalen < 6 and \p fillch != '\0', the output is padded with
 *    two \p fillch characters per missing byte until 6 groups are produced,
 *    guaranteeing a minimum length of 17 characters (HH:HH:HH:HH:HH:HH).
 *  - If \p datalen < 6 and \p fillch == '\0', no padding is added; the output
 *    contains only the provided bytes (resulting in fewer than 6 groups).
 *  - If \p data is nullptr, it is treated as zero-length input.
 *
 * \param data     Pointer to the input byte array.
 * \param datalen  Number of valid bytes in \p data.
 * \param fillch   Padding character for missing bytes when \p datalen < 6.
 *                 If '\0', no padding is applied.
 *
 * \return Formatted MAC-style string. Marked [[nodiscard]] because ignoring
 *         the returned string is typically unintended.
 */
[[nodiscard]]
std::string to_mac_string(const uint8_t* data, size_t datalen, char fillch = '?');

#endif /* TOOLS_H */
