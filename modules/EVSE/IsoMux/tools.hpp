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
#include <sys/time.h>
#include <time.h>

enum Addr6Type {
    ADDR6_TYPE_UNPSEC = -1,
    ADDR6_TYPE_GLOBAL = 0,
    ADDR6_TYPE_LINKLOCAL = 1,
};

const char* choose_first_ipv6_interface();
int get_interface_ipv6_address(const char* if_name, enum Addr6Type type, struct sockaddr_in6* addr);

void set_normalized_timespec(struct timespec* ts, time_t sec, int64_t nsec);
struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);
long long timespec_to_ms(struct timespec ts);

/*!
 * \brief get_dir_filename This function searches for a specific name (AFileNameIdentifier) in a file path and stores
 * the complete name with file ending in \c AFileName
 * \param file_name is the buffer to write the file name.
 * \param file_name_len is the length of the buffer.
 * \param path is the file path which will be used to search for the specific file.
 * \param file_name_identifier is the identifier of the file (file without file ending).
 * \return Returns \c true if the file could be found, otherwise \c false.
 */
bool get_dir_filename(char* file_name, uint8_t file_name_len, const char* path, const char* file_name_identifier);

/**
 * \brief convert the given \p hash_algorithm to type types::iso15118::HashAlgorithm
 * \param hash_algorithm
 * \return types::iso15118::HashAlgorithm
 */
types::iso15118::HashAlgorithm convert_to_hash_algorithm(const types::evse_security::HashAlgorithm hash_algorithm);

#endif /* TOOLS_H */
