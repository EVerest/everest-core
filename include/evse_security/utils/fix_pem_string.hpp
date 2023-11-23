// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <regex>
#include <string>

class MalformedPEMException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

static std::regex not_b64_chars("[^a-zA-Z0-9/\\+=]");
static std::regex single_pem("(-----BEGIN[^-]*-----)([^-]*)(-----END[^-]*-----\\n?)");

static std::string fix_pem_payload(const std::string& input_pem_payload) {
    // Remove all newlines and whitespace from the rest
    std::string cleared_string = std::regex_replace(input_pem_payload, not_b64_chars, "");

    // Start with a newline, split the string into 64-char chunks, add newline after each
    std::string result = "\n";
    for (std::string::size_type pos = 0; pos < cleared_string.size(); pos += 64) {
        std::string chunk = cleared_string.substr(pos, 64);
        result += chunk;
        result += "\n";
    }
    return result;
}

static std::string fix_pem_string(const std::string& input_pem_string) {
    std::string header;
    std::string footer;
    std::string payload;

    // Split the string into header, footer, and b64 encoding
    std::smatch matches;
    try {
        std::regex_match(input_pem_string, matches, single_pem);
    } catch (std::regex_error& e) {
        throw MalformedPEMException("Invalid PEM string: " + input_pem_string);
    }
    if (matches.size() != 4) {
        throw MalformedPEMException("Invalid PEM string: " + input_pem_string);
    }
    header = matches[1];
    payload = matches[2];
    footer = matches[3];

    // Fixup the payload
    payload = fix_pem_payload(payload);

    // return the result
    return header + payload + footer;
}
