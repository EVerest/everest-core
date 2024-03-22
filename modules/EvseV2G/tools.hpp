// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#ifndef TOOLS_H
#define TOOLS_H

#include <generated/types/evse_security.hpp>
#include <generated/types/iso15118_charger.hpp>
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

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ROUND_UP
#define ROUND_UP(N, S) ((((N) + (S)-1) / (S)) * (S))
#endif

#ifndef ROUND_UP_ELEMENTS
#define ROUND_UP_ELEMENTS(N, S) (((N) + (S)-1) / (S))
#endif

int generate_random_data(void* dest, size_t dest_len);
unsigned int generate_srand_seed(void);

enum Addr6Type {
    ADDR6_TYPE_UNPSEC = -1,
    ADDR6_TYPE_GLOBAL = 0,
    ADDR6_TYPE_LINKLOCAL = 1,
};

const char* choose_first_ipv6_interface();
int get_interface_ipv6_address(const char* if_name, enum Addr6Type type, struct sockaddr_in6* addr);

void set_normalized_timespec(struct timespec* ts, time_t sec, int64_t nsec);
int timespec_compare(const struct timespec* lhs, const struct timespec* rhs);
struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);
struct timespec timespec_add(struct timespec lhs, struct timespec rhs);
void timespec_add_ms(struct timespec* ts, long long msec);
long long timespec_to_ms(struct timespec ts);
long long timespec_to_us(struct timespec ts);
int msleep(int ms);
long long int getmonotonictime(void);

/*!
 *  \brief calc_physical_value This function calculates the physical value consists on a value and multiplier.
 *  \param value is the value of the physical value
 *  \param multiplier is the multiplier of the physical value
 *  \return Returns the physical value
 */
double calc_physical_value(const int16_t& value, const int8_t& multiplier);

/*!
 * \brief range_check_int32 This function checks if an int32 value is within the given range.
 * \param min is the min value.
 * \param max is the max value.
 * \param value which must be checked.
 * \return Returns \c true if it is within range, otherwise \c false.
 */
bool range_check_int32(int32_t min, int32_t max, int32_t value);

/*!
 * \brief range_check_int64 This function checks if an int64 value is within the given range.
 * \param min is the min value.
 * \param max is the max value.
 * \param value which must be checked.
 * \return Returns \c true if it is within range, otherwise \c false.
 */
bool range_check_int64(int64_t min, int64_t max, int64_t value);

/*!
 * \brief round_down "round" a string representation of a float down to 1 decimal places
 * \param buffer is the float string
 * \param len is the length of the buffer
 */
void round_down(const char* buffer, size_t len);

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

/*!
 * \brief get_dir_numbered_file_names This helper-function searches for numbered files in the given file path and stores
 * the file names in given char array
 * \param file_names is the char array for the findings.
 * \param path is the path where the numbered files are stored.
 * \param prefix is the prefix of the numbered file.
 * \param suffix is the suffix of the numbered file.
 * \param offset defines the starting number of the file name.
 * \param max_idx is the max index of a file (Between 0-9).
 * \return Returns the number of files which where found in the file path.
 */
uint8_t get_dir_numbered_file_names(char file_names[MAX_PKI_CA_LENGTH][MAX_FILE_NAME_LENGTH], const char* path,
                                    const char* prefix, const char* suffix, const uint8_t offset,
                                    const uint8_t max_idx);

/*!
 * \brief convert_to_hex_str This function converts a array of binary data to hex string.
 * \param data is the array of binary data.
 * \param len is length of the array.
 * \return Returns the converted string.
 */
std::string convert_to_hex_str(const uint8_t* data, int len);

/**
 * \brief convert the given \p hash_algorithm to type types::iso15118_charger::HashAlgorithm
 * \param hash_algorithm
 * \return types::iso15118_charger::HashAlgorithm
 */
types::iso15118_charger::HashAlgorithm
convert_to_hash_algorithm(const types::evse_security::HashAlgorithm hash_algorithm);

/**
 * \brief convert the given \p ocsp_request_data_list to std::vector<types::iso15118_charger::CertificateHashDataInfo>
 * \param ocsp_request_data_list
 * \return std::vector<types::iso15118_charger::CertificateHashDataInfo>
 */
std::vector<types::iso15118_charger::CertificateHashDataInfo>
convert_to_certificate_hash_data_info_vector(const types::evse_security::OCSPRequestDataList& ocsp_request_data_list);

#endif /* TOOLS_H */
