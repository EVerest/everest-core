// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef YETI_FIRMWARE_VERSION
#define YETI_FIRMWARE_VERSION

#include <regex>
#include <string>

/*
  Helper class to handle yeti Firmware Versions of the following style:

  Reported by MCU: 2.0-1-g2d51638
  Included in file name: yetiR1_2.0-1_firmware.bin

*/

static std::vector<std::string> split_by_delimeters(const std::string& s, const std::string& delimeters) {
    std::regex re("[" + delimeters + "]");
    std::sregex_token_iterator first{s.begin(), s.end(), re, -1}, last;
    return {first, last};
}

// parse version string from filename
static std::string from_filename(const std::string& v) {
    auto tokens = split_by_delimeters(v, "_");
    if (tokens.size() >= 2) {
        return tokens[1];
    }
    return "0.0-0";
}

class YetiFirmwareVersion {

public:
    YetiFirmwareVersion() {
    }

    YetiFirmwareVersion(const std::string& _version_string) {
        from_string(_version_string);
    }

    std::string to_string() {
        return std::to_string(major) + "." + std::to_string(minor) + "-" + std::to_string(revision);
    }

    void from_string(const std::string& _version_string) {
        std::string version_string;
        // Is it a full filename or just a version string from the MCU?
        if (_version_string.rfind("yetiR", 0) == 0) {
            version_string = from_filename(_version_string);
        } else {
            version_string = _version_string;
        }

        auto tokens = split_by_delimeters(version_string, ".-");

        // parse into major, minor and revision
        if (tokens.size() >= 1) {
            major = std::stoi(tokens[0]);
        }

        if (tokens.size() >= 2) {
            minor = std::stoi(tokens[1]);
        }

        if (tokens.size() >= 3) {
            revision = std::stoi(tokens[2]);
        }
    }

    YetiFirmwareVersion& operator=(const std::string& s) {
        from_string(s);
        return *this;
    }

    friend bool operator<(const YetiFirmwareVersion& l, const YetiFirmwareVersion& r) {
        if (l.major < r.major) {
            return true;
        } else if (l.major > r.major) {
            return false;
        } else if (l.minor < r.minor) {
            return true;
        } else if (l.minor > r.minor) {
            return false;
        } else if (l.revision < r.revision) {
            return true;
        } else if (l.revision > r.revision) {
            return false;
        }
        // they are identical
        return false;
    }

    friend bool operator>(const YetiFirmwareVersion& lhs, const YetiFirmwareVersion& rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(const YetiFirmwareVersion& lhs, const YetiFirmwareVersion& rhs) {
        return not(lhs > rhs);
    }
    friend bool operator>=(const YetiFirmwareVersion& lhs, const YetiFirmwareVersion& rhs) {
        return not(lhs < rhs);
    }

private:
    int major{0};
    int minor{0};
    int revision{0};
};

#endif
