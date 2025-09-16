// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

namespace everest::lib::io::event {

/**
 * unique_fd manages the lifetime of a file descriptor. It takes ownership of the held filedescriptor and * closes it on
 * destruction. Objects of this class can be moved but not copied.
 */
class unique_fd {
public:
    unique_fd() = default;

    /**
     * @brief Construct from file descriptor
     * @details Takes ownership of the file descriptor. Implicit conversion is disabled.
     * @param[in] fd A valid filedescriptor.
     */
    explicit unique_fd(int fd);
    unique_fd(const unique_fd&) = delete;
    unique_fd& operator=(const unique_fd&) = delete;

    /**
     * @brief Construct from other unique_fd.
     * @param[in, out] other Transfer ownership from \p other to this.
     */
    unique_fd(unique_fd&& other);
    /**
     * @brief Assign from other unique_fd.
     * @param[in, out] other Transfer ownership from \p other to this.
     */
    unique_fd& operator=(unique_fd&& other);

    /**
     * @brief Destructor calls \ref close internally
     */
    ~unique_fd();

    /**
     * @brief Conversion to file descriptor
     * @details The conversion is implicit
     * @return The managed file descriptor
     */
    operator int() const;

    /**
     * @brief Check if a file descriptor is held
     * @details Compares to \ref NO_DESCRIPTOR_SENTINEL
     * @return True if a value is held, false otherwise
     */
    bool is_fd() const;

    /**
     * @brief Give up ownership of the file descriptor
     * @return The previously managed file descriptor
     */
    int release();

    /**
     * @brief Close the file descriptor
     * @details \ref is_fd return false after this call.
     */
    void close();

    /**
     * @var NO_DESCRIPTOR_SENTINEL
     * @brief Special value representing an invalid file descriptor
     */
    constexpr static int NO_DESCRIPTOR_SENTINEL = -1;

private:
    int m_fd{NO_DESCRIPTOR_SENTINEL};
};
} // namespace everest::lib::io::event
