// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

/**
 * \file convert an enum into bit flags
 * \note enum must contain item "last" which has the highest value
 */

#ifndef ENUMFLAGS_HPP
#define ENUMFLAGS_HPP

#include <atomic>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace everest::staging::util {

/**
 * \brief templated class to use an enumeration as bit flags
 * \note Enumeration must have the last element called last
 *
 * Example:
 * \code
 * enum class example : std::uin8_t {
 *     item1,
 *     item2,
 *     item3,
 *     last = item3,
 * };
 *
 * util::EnumFlags<example> flags;
 *
 * flags.set(example::item1);
 * flags.is_set(example::item1); // true
 * flags.reset(example::item1);
 * flags.is_set(example::item1); // false
 * \endcode
 *
 * Multiple flags can be combined:
 * \code
 * flags.reset();
 * flags.set(example::item1, example::item2);
 * flags.is_set(example::item1, example::item2); // true
 * flags.is_set(example::item3, example::item2); // false
 * flags.is_any_set(example::item3, example::item2); // true
 * \endcode
 */

template <typename T>
using SelectedUInt = std::conditional_t<
    (static_cast<std::size_t>(T::last) < 8), std::uint8_t,
    std::conditional_t<(static_cast<std::size_t>(T::last) < 16), std::uint16_t,
                       std::conditional_t<(static_cast<std::size_t>(T::last) < 32), std::uint32_t,
                                          std::conditional_t<(static_cast<std::size_t>(T::last) < 64), std::uint64_t,
                                                             void // invalid, triggers static_assert below
                                                             >>>>;

template <typename T, typename B> class EnumFlagsBase {
public:
    static_assert(std::is_enum<T>(), "Not enum");
    static_assert(std::is_integral<SelectedUInt<T>>(), "Not supported");

private:
    B _value{0ULL};

    constexpr auto max_value() const {
        if constexpr (static_cast<std::underlying_type_t<T>>(T::last) == 64) {
            return std::numeric_limits<std::uint64_t>::max();
        } else {
            return (1ULL << (static_cast<std::underlying_type_t<T>>(T::last) + 1)) - 1;
        }
    }

public:
    /**
     * \brief return the bit position for the specified enum value
     * \param[in] flag the enum value
     * \returns an unsigned integer with the equivalent bit set
     */
    static constexpr std::size_t bit(T flag) {
        return 1ULL << static_cast<std::underlying_type_t<T>>(flag);
    }

    /**
     * \brief set the state of a specific flag
     * \param[in] flag - the enum value to update
     * \param[in] value - set/reset the flag
     */
    constexpr void set(T flag, bool value) {
        if (value) {
            set(flag);
        } else {
            reset(flag);
        }
    }

    /**
     * \brief set the specific flag
     * \param[in] flag - the enum value to set
     */
    constexpr void set(T flag) {
        _value |= bit(flag);
    }

    /**
     * \brief set flags to a specific value
     * \note not recommended
     */
    constexpr void set(std::underlying_type_t<T> v) {
        _value = v & max_value();
    }

    /**
     * \brief same as above but for multiple flags
     */
    template <typename... Flags> constexpr void set(T flag, const Flags... flags) {
        set(flag);
        set(flags...);
    }

    /**
     * \brief reset the specific flag
     * \param[in] flag - the enum value to reset
     */
    constexpr void reset(T flag) {
        _value &= ~bit(flag);
    }

    /**
     * \brief same as above but for multiple flags
     */
    template <typename... Flags> constexpr void reset(T flag, const Flags... flags) {
        reset(flag);
        reset(flags...);
    }

    /**
     * \brief reset all flags
     */
    constexpr void reset() {
        _value = 0ULL;
    }

    /**
     * \brief set all flags
     */
    constexpr void set() {
        _value = max_value();
    }

    /**
     * \brief test if all flags are reset (i.e. 0)
     * \returns true when no flag is set
     */
    [[nodiscard]] constexpr bool all_reset() const {
        return _value == 0ULL;
    }

    /**
     * \brief test if any flags are set
     * \returns true when any flag is set
     */
    [[nodiscard]] constexpr bool any_reset() const {
        return _value != max_value();
    }

    /**
     * \brief test if all flags are reset (i.e. 0)
     * \returns true when no flag is set
     */
    [[nodiscard]] constexpr bool all_set() const {
        return _value == max_value();
    }

    /**
     * \brief test if any flags are set
     * \returns true when any flag is set
     */
    [[nodiscard]] constexpr bool any_set() const {
        return _value != 0ULL;
    }

    /**
     * \brief test if a flag is set
     * \returns true when the flag is set
     */
    constexpr bool is_set(T flag) const {
        return (_value & bit(flag)) != 0;
    }
    constexpr bool is_any_set(T flag) const {
        return is_set(flag);
    }

    /**
     * \brief test if a flag is reset
     * \returns true when the flag is reset
     */
    constexpr bool is_reset(T flag) const {
        return (_value & bit(flag)) == 0;
    }
    constexpr bool is_any_reset(T flag) const {
        return is_reset(flag);
    }

    /**
     * \brief retrieve all flags
     * \returns the internal _value
     */
    constexpr auto get() const {
        return _value;
    }

    /**
     * \brief same as above but for multiple flags
     * \note is_set() all specified flags must be set for a true result
     * \note is_any_set() at least one of the specified flags must be set for a true result
     * \note is_reset() all specified flags must be reset for a true result
     * \note is_any_reset() at least one of the specified flags must be reset for a true result
     */
    template <typename... Flags> constexpr bool is_set(T flag, const Flags... flags) const {
        return is_set(flag) && is_set(flags...);
    }

    template <typename... Flags> constexpr bool is_any_set(T flag, const Flags... flags) const {
        return is_any_set(flag) || is_any_set(flags...);
    }

    template <typename... Flags> constexpr bool is_reset(T flag, const Flags... flags) const {
        return is_reset(flag) && is_reset(flags...);
    }

    template <typename... Flags> constexpr bool is_any_reset(T flag, const Flags... flags) const {
        return is_any_reset(flag) || is_any_reset(flags...);
    }
};

template <typename T> struct EnumFlags : public EnumFlagsBase<T, SelectedUInt<T>> {};

template <typename T> struct AtomicEnumFlags : public EnumFlagsBase<T, std::atomic<SelectedUInt<T>>> {};

} // namespace everest::staging::util

#endif
