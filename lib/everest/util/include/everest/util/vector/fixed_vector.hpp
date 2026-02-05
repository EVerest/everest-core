// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

/**
 * @file fixed_vector.hpp
 * @brief Provides a std::vector-like container with fixed capacity, avoiding dynamic memory allocation.
 *
 * The fixed_vector is a sequence container that encapsulates a fixed-size array. It provides an interface
 * similar to std::vector but does not allocate memory on the heap. Its capacity is determined at compile time
 * by the template parameter N. This makes it suitable for real-time and embedded applications where
 * dynamic memory allocation is disallowed or undesirable.
 */

#pragma once

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace everest::lib::util {

/**
 * @brief A container with std::vector-like interface but with fixed capacity.
 *
 * This class mimics the behavior of std::vector but stores its elements in a fixed-size internal buffer,
 * avoiding heap allocations. The capacity is specified at compile time.
 * If the number of elements exceeds the capacity, it results in an exception (`std::length_error`).
 *
 * @tparam T The type of elements.
 * @tparam N The maximum number of elements the vector can hold (its capacity).
 */
template <typename T, std::size_t N> class fixed_vector {
public:
    //- Member types
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * @brief Constructs an empty fixed_vector.
     */
    constexpr fixed_vector() noexcept = default;

    /**
     * @brief Destroys the fixed_vector, calling destructors for all contained elements.
     */
    ~fixed_vector() {
        clear();
    }

    /**
     * @brief Copy constructor. Constructs the vector with a copy of the contents of other.
     * @param other another fixed_vector object to be used as source to initialize the elements of the container with.
     */
    fixed_vector(const fixed_vector& other) {
        copy_construct_from(other);
    }

    /**
     * @brief Move constructor. Constructs the vector with the contents of other using move semantics.
     * @details This constructor has two behaviors depending on `T`:
     *          1. If `T` is `nothrow_move_constructible`, this operation is `noexcept` and highly efficient.
     *          2. If `T`'s move constructor can throw, this provides a strong guarantee: if a failure occurs, the
     *             source `other` is restored to its original state. However, the exception is **swallowed**, and
     * `*this` will be empty. The caller must be prepared to handle an empty vector even if `other` was not.
     * @param other another fixed_vector object to be used as source.
     */
    fixed_vector(fixed_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        move_construct_from(std::move(other));
    }

    /**
     * @brief Constructs the vector with the contents of the initializer list.
     * @param init initializer list to initialize the elements of the container with.
     * @throws std::length_error if the size of the initializer list is greater than the capacity of the vector.
     */
    fixed_vector(std::initializer_list<T> init) {
        if (init.size() > N) {
            throw std::length_error("Initializer list size exceeds fixed_vector capacity");
        }

        // std::uninitialized_copy constructs elements in-place. If an element's
        // constructor throws, it destroys any elements already created.
        // We only update `size_` after all elements are successfully constructed,
        // which provides the strong exception guarantee.
        std::uninitialized_copy(init.begin(), init.end(), data());
        size_ = init.size();
    }

    /**
     * @brief Constructs the vector with the contents of an std::vector.
     * @param vec std::vector to initialize the elements of the container with.
     * @throws std::length_error if the size of the std::vector is greater than the capacity of the fixed_vector.
     */
    explicit fixed_vector(const std::vector<T>& vec) {
        if (vec.size() > N) {
            throw std::length_error("std::vector size exceeds fixed_vector capacity");
        }

        std::uninitialized_copy(vec.begin(), vec.end(), data());
        size_ = vec.size();
    }

    /**
     * @brief Copy assignment operator. Replaces the contents with a copy of the contents of other.
     * @param other another fixed_vector object to be used as source.
     * @return *this
     */
    fixed_vector& operator=(const fixed_vector& other) {
        if (this != &other) {
            copy_assign_from(other);
        }
        return *this;
    }

    /**
     * @brief Move assignment operator. Replaces the contents with those of other using move semantics.
     * @details If `T`'s move operations can throw, this provides a hybrid exception guarantee: if an exception is
     * thrown, the source `other` is restored to its original state, while `*this` is left in a valid but
     * unspecified state. **However, the exception is swallowed, and not re-thrown.** If move operations are `nothrow`,
     * this is `noexcept`. The caller must be prepared for `*this` to be in a valid but unspecified state without an
     * exception being propagated.
     * @param other another fixed_vector object to be used as source.
     * @return *this
     */
    fixed_vector& operator=(fixed_vector&& other) noexcept(
        std::is_nothrow_move_assignable_v<T>&& std::is_nothrow_move_constructible_v<T>) {
        if (this != &other) {
            move_assign_from(std::move(other));
        }
        return *this;
    }

    // Element access
    /**
     * @brief Returns a reference to the element at specified location `pos`, with bounds checking.
     * @param pos position of the element to return.
     * @return Reference to the requested element.
     * @throws std::out_of_range if `pos >= size()`.
     */
    constexpr reference at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range("fixed_vector::at");
        }
        return data()[pos];
    }

    /**
     * @brief Returns a const reference to the element at specified location `pos`, with bounds checking.
     * @param pos position of the element to return.
     * @return Const reference to the requested element.
     * @throws std::out_of_range if `pos >= size()`.
     */
    constexpr const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw std::out_of_range("fixed_vector::at");
        }
        return data()[pos];
    }

    /**
     * @brief Returns a reference to the element at specified location `pos`. No bounds checking is performed.
     * @param pos position of the element to return.
     * @return Reference to the element at `pos`.
     */
    constexpr reference operator[](size_type pos) {
        return data()[pos];
    }

    /**
     * @brief Returns a const reference to the element at specified location `pos`. No bounds checking is performed.
     * @param pos position of the element to return.
     * @return Const reference to the element at `pos`.
     */
    constexpr const_reference operator[](size_type pos) const {
        return data()[pos];
    }

    /**
     * @brief Returns a reference to the first element in the container.
     * @details Calling front on an empty container is undefined.
     * @return Reference to the first element.
     */
    constexpr reference front() {
        return data()[0];
    }

    /**
     * @brief Returns a const reference to the first element in the container.
     * @details Calling front on an empty container is undefined.
     * @return Const reference to the first element.
     */
    constexpr const_reference front() const {
        return data()[0];
    }

    /**
     * @brief Returns a reference to the last element in the container.
     * @details Calling back on an empty container is undefined.
     * @return Reference to the last element.
     */
    constexpr reference back() {
        return data()[size_ - 1];
    }

    /**
     * @brief Returns a const reference to the last element in the container.
     * @details Calling back on an empty container is undefined.
     * @return Const reference to the last element.
     */
    constexpr const_reference back() const {
        return data()[size_ - 1];
    }

    // Iterators
    /**
     * @brief Returns an iterator to the first element of the vector.
     * @return Iterator to the first element.
     */
    constexpr iterator begin() noexcept {
        return data();
    }
    /**
     * @brief Returns a const iterator to the first element of the vector.
     * @return Const iterator to the first element.
     */
    constexpr const_iterator begin() const noexcept {
        return data();
    }
    /**
     * @brief Returns a const iterator to the first element of the vector.
     * @return Const iterator to the first element.
     */
    constexpr const_iterator cbegin() const noexcept {
        return data();
    }

    /**
     * @brief Returns an iterator to the element following the last element of the vector.
     * @return Iterator to the element following the last element.
     */
    constexpr iterator end() noexcept {
        return data() + size_;
    }
    /**
     * @brief Returns a const iterator to the element following the last element of the vector.
     * @return Const iterator to the element following the last element.
     */
    constexpr const_iterator end() const noexcept {
        return data() + size_;
    }
    /**
     * @brief Returns a const iterator to the element following the last element of the vector.
     * @return Const iterator to the element following the last element.
     */
    constexpr const_iterator cend() const noexcept {
        return data() + size_;
    }

    /**
     * @brief Returns a reverse iterator to the first element of the reversed vector.
     * @return Reverse iterator to the first element.
     */
    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    /**
     * @brief Returns a const reverse iterator to the first element of the reversed vector.
     * @return Const reverse iterator to the first element.
     */
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    /**
     * @brief Returns a const reverse iterator to the first element of the reversed vector.
     * @return Const reverse iterator to the first element.
     */
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    /**
     * @brief Returns a reverse iterator to the element following the last element of the reversed vector.
     * @return Reverse iterator to the element following the last element.
     */
    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    /**
     * @brief Returns a const reverse iterator to the element following the last element of the reversed vector.
     * @return Const reverse iterator to the element following the last element.
     */
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    /**
     * @brief Returns a const reverse iterator to the element following the last element of the reversed vector.
     * @return Const reverse iterator to the element following the last element.
     */
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // Capacity
    /**
     * @brief Checks if the container has no elements.
     * @return true if the container is empty, false otherwise.
     */
    [[nodiscard]] constexpr bool empty() const noexcept {
        return size_ == 0;
    }
    /**
     * @brief Returns the number of elements in the container.
     * @return The number of elements in the container.
     */
    [[nodiscard]] constexpr size_type size() const noexcept {
        return size_;
    }
    /**
     * @brief Returns the maximum number of elements the container is able to hold.
     * @return The maximum number of elements.
     */
    [[nodiscard]] constexpr size_type max_size() const noexcept {
        return N;
    }
    /**
     * @brief Returns the number of elements that the container has currently allocated space for. For fixed_vector,
     * this is always N.
     * @return The capacity of the container.
     */
    [[nodiscard]] constexpr size_type capacity() const noexcept {
        return N;
    }

    // Modifiers
    /**
     * @brief Erases all elements from the container.
     */
    void clear() noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (size_type i = 0; i < size_; ++i) {
                std::destroy_at(data() + i);
            }
        }
        size_ = 0;
    }

    /**
     * @brief Appends a new element to the end of the container, constructed in-place.
     * @param args arguments to forward to the constructor of the element.
     * @return Reference to the emplaced element.
     * @throws std::length_error if the container is full.
     */
    template <class... Args> reference emplace_back(Args&&... args) {
        if (size_ >= N) {
            throw std::length_error("fixed_vector is full");
        }
        pointer ptr = data() + size_;
        new (ptr) T(std::forward<Args>(args)...);
        ++size_;
        return *ptr;
    }

    /**
     * @brief Appends a new element to the end of the container, constructed in-place, without throwing exceptions.
     * @details If the container is full, or if the constructor of the element throws, this function does nothing
     *          and returns `nullptr`.
     * @param args arguments to forward to the constructor of the element.
     * @return A pointer to the new element if successful, or `nullptr` otherwise.
     */
    template <class... Args> pointer try_emplace_back(Args&&... args) noexcept {
        try {
            return &emplace_back(std::forward<Args>(args)...);
        } catch (...) {
            return nullptr;
        }
    }

    /**
     * @brief Appends the given element `value` to the end of the container.
     * @param value the value of the element to append.
     * @throws std::length_error if the container is full.
     */
    void push_back(const T& value) {
        emplace_back(value);
    }

    /**
     * @brief Appends the given element `value` to the end of the container using move semantics.
     * @param value the value of the element to append.
     * @throws std::length_error if the container is full.
     */
    void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    /**
     * @brief Removes the last element of the container.
     * @details Calling pop_back on an empty container is a no-op in this implementation.
     */
    void pop_back() {
        if (size_ > 0) {
            --size_;
            if constexpr (!std::is_trivially_destructible_v<T>) {
                std::destroy_at(data() + size_);
            }
        }
    }

    /**
     * @brief Erases the element at `pos`.
     * @param pos iterator to the element to remove.
     * @return Iterator following the last removed element.
     */
    iterator erase(const_iterator pos) {
        const auto offset = std::distance(cbegin(), pos);
        iterator ita = begin() + offset;

        std::move(ita + 1, end(), ita);

        pop_back();

        return ita;
    }

    /**
     * @brief Erases the elements in the range `[first, last)`.
     * @param first the first element to remove.
     * @param last the last element to remove.
     * @return Iterator following the last removed element.
     */
    iterator erase(const_iterator first, const_iterator last) {
        iterator it_first = begin() + std::distance(cbegin(), first);
        iterator it_last = begin() + std::distance(cbegin(), last);
        auto count = std::distance(it_first, it_last);

        if (count > 0) {
            std::move(it_last, end(), it_first);

            auto new_size = size_ - count;
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for (size_type i = new_size; i < size_; ++i) {
                    std::destroy_at(data() + i);
                }
            }
            size_ = new_size;
        }

        return it_first;
    }

private:
    alignas(T) std::array<std::byte, N * sizeof(T)> storage_; ///< Internal storage for elements.
    size_type size_{0};                                       ///< Current number of elements.

    // --- Implementation helpers for special members ---

    // Copy-construct
    /**
     * @brief Helper for copy construction: constructs this fixed_vector by copy-constructing elements from another.
     * @details This overload is enabled if `T` is copy-constructible.
     * @param other The fixed_vector to copy elements from.
     */
    template <typename U = T, std::enable_if_t<std::is_copy_constructible_v<U>>* = nullptr>
    void copy_construct_from(const fixed_vector& other) {
        for (const auto& elem : other) {
            push_back(elem);
        }
    }

    /**
     * @brief Helper for copy construction: provides a compile-time error if `T` is not copy-constructible.
     * @details This overload is enabled if `T` is not copy-constructible, triggering a `static_assert`.
     * @param other Unused, present for signature matching.
     */
    template <typename U = T, std::enable_if_t<!std::is_copy_constructible_v<U>>* = nullptr>
    void copy_construct_from(const fixed_vector& /*other*/) {
        static_assert(std::is_copy_constructible_v<U>,
                      "fixed_vector requires T to be copy-constructible for copy construction.");
    }

    // Copy-assign
    /**
     * @brief Helper for copy assignment: assigns elements from another fixed_vector.
     * @details This overload is enabled if `T` is both copy-constructible and copy-assignable.
     *          It uses an efficient element-wise assignment strategy.
     * @param other The fixed_vector to assign elements from.
     * @return A reference to this fixed_vector.
     */
    template <typename U = T,
              std::enable_if_t<std::is_copy_constructible_v<U> && std::is_copy_assignable_v<U>>* = nullptr>
    fixed_vector& copy_assign_from(const fixed_vector& other) {
        const size_type copy_len = std::min(size_, other.size_);
        std::copy(other.begin(), other.begin() + copy_len, begin());

        if (size_ < other.size_) {
            for (size_type i = size_; i < other.size_; ++i) {
                push_back(other[i]);
            }
        } else if (size_ > other.size_) {
            const size_type old_size = size_;
            size_ = other.size_;
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for (size_type i = size_; i < old_size; ++i) {
                    std::destroy_at(data() + i);
                }
            }
        }
        return *this;
    }

    /**
     * @brief Helper for copy assignment: provides a compile-time error if `T` is not copy-constructible or not
     * copy-assignable.
     * @details This overload is enabled if `T` does not meet the requirements, triggering `static_assert`s.
     * @param other Unused, present for signature matching.
     * @return A reference to this fixed_vector (never reached due to static_assert).
     */
    template <typename U = T,
              std::enable_if_t<!(std::is_copy_constructible_v<U> && std::is_copy_assignable_v<U>)>* = nullptr>
    fixed_vector& copy_assign_from(const fixed_vector& /*other*/) {
        static_assert(std::is_copy_constructible_v<U>,
                      "fixed_vector requires T to be copy-constructible for copy assignment.");
        static_assert(std::is_copy_assignable_v<U>,
                      "fixed_vector requires T to be copy-assignable for copy assignment.");
        return *this;
    }

    // Move-construct
    /**
     * @brief Helper for move construction (fast path).
     * @details This overload is enabled if `T` is nothrow move-constructible. It is `noexcept` and does not
     *          provide a rollback, as failure is not expected.
     */
    template <typename U = T, std::enable_if_t<std::is_nothrow_move_constructible_v<U>>* = nullptr>
    void move_construct_from(fixed_vector&& other) noexcept {
        for (auto& elem : other) {
            if (try_emplace_back(std::move(elem)) == nullptr) {
                // This vector is full, or move ctor threw. In nothrow-case, it must be full.
                break;
            }
        }
        other.clear();
    }

    /**
     * @brief Helper for move construction (strong guarantee path).
     * @details This overload is enabled if `T`'s move constructor can throw. It provides the strong guarantee
     *          by moving elements back to the source if a failure occurs. This requires `T` to be move-assignable.
     *          **Important: This function swallows any exception during element-wise construction and does not
     *          propagate it.**
     */
    template <typename U = T, std::enable_if_t<(!std::is_nothrow_move_constructible_v<U> &&
                                                std::is_move_constructible_v<U>)>* = nullptr>
    void move_construct_from(fixed_vector&& other) {
        static_assert(std::is_move_assignable_v<U>,
                      "T must be move-assignable to provide strong exception guarantee for throwing move constructor");

        if (this == &other) {
            return;
        }

        size_type moved_count = 0;
        try {
            for (auto& elem : other) {
                emplace_back(std::move(elem));
                ++moved_count;
            }
            other.clear();
        } catch (...) {
            // A move construction failed. Restore the source `other`.
            for (size_type i = 0; i < moved_count; ++i) {
                other[i] = std::move((*this)[i]);
            }
            // Clear this vector, which now contains moved-from objects.
            this->clear();
        }
    }

    /**
     * @brief Helper for move construction: provides a compile-time error if `T` is not move-constructible.
     * @details This overload is enabled if `T` is not move-constructible, triggering a `static_assert`.
     * @param other Unused, present for signature matching.
     */
    template <typename U = T, std::enable_if_t<!std::is_move_constructible_v<U>>* = nullptr>
    void move_construct_from(fixed_vector&& /*other*/) {
        static_assert(std::is_move_constructible_v<U>,
                      "fixed_vector requires T to be move-constructible for move construction.");
    }

    // Move-assign
    /**
     * @brief Helper for move assignment (fast path).
     * @details This overload is enabled if `T`'s move operations are nothrow. It is `noexcept`.
     */
    template <typename U = T, std::enable_if_t<(std::is_nothrow_move_assignable_v<U> &&
                                                std::is_nothrow_move_constructible_v<U>)>* = nullptr>
    void move_assign_from(fixed_vector&& other) noexcept {
        const size_type move_len = std::min(size_, other.size_);
        std::move(other.begin(), other.begin() + move_len, begin()); // move assign

        if (size_ < other.size_) {
            for (size_type i = size_; i < other.size_; ++i) {
                if (try_emplace_back(std::move(other[i])) == nullptr) { // move construct
                    // This vector is full, or move ctor threw. In nothrow-case, it must be full.
                    break;
                }
            }
        } else if (size_ > other.size_) {
            const size_type old_size = size_;
            size_ = other.size_;
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for (size_type i = size_; i < old_size; ++i) {
                    std::destroy_at(data() + i);
                }
            }
        }
        other.clear();
    }

    /**
     * @brief Helper for move assignment (hybrid exception guarantee path).
     * @details This overload is enabled if `T`'s move operations can throw. If an exception occurs,
     *          the source `other` is restored to its original state, while `this` is left in a valid but
     *          unspecified state. **Important: Any exception thrown during element operations is swallowed,
     *          and not re-thrown.**
     */
    template <typename U = T,
              std::enable_if_t<!(std::is_nothrow_move_assignable_v<U> &&
                                 std::is_nothrow_move_constructible_v<U>)&&std::is_move_assignable_v<U> &&
                               std::is_move_constructible_v<U>>* = nullptr>
    void move_assign_from(fixed_vector&& other) {
        if (this == &other) {
            return;
        }

        const size_type original_this_size = this->size_;
        size_type assigned_count = 0;
        size_type constructed_count = 0;

        try {
            // Move-assign over existing elements
            const size_type assign_len = std::min(original_this_size, other.size_);
            for (assigned_count = 0; assigned_count < assign_len; ++assigned_count) {
                (*this)[assigned_count] = std::move(other[assigned_count]);
            }

            // Move-construct new elements if `this` is growing
            if (original_this_size < other.size_) {
                for (size_type i = original_this_size; i < other.size_; ++i) {
                    emplace_back(std::move(other[i]));
                    ++constructed_count;
                }
            } else if (original_this_size > other.size_) {
                // Destroy extra elements if `this` is shrinking
                size_ = other.size_;
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    for (size_type i = size_; i < original_this_size; ++i) {
                        std::destroy_at(data() + i);
                    }
                }
            }
            other.clear();
        } catch (...) {
            // Failure occurred. Restore `other`.
            for (size_type i = 0; i < assigned_count; ++i) {
                other[i] = std::move((*this)[i]);
            }
            for (size_type i = 0; i < constructed_count; ++i) {
                other[original_this_size + i] = std::move((*this)[original_this_size + i]);
            }
            // Note: `this` is left in a valid but unspecified state.
        }
    }

    /**
     * @brief Helper for move assignment: provides a compile-time error if `T` is not move-constructible or not
     * move-assignable.
     * @details This overload is enabled if `T` does not meet the requirements, triggering `static_assert`s.
     * @param other Unused, present for signature matching.
     */
    template <typename U = T,
              std::enable_if_t<!(std::is_move_constructible_v<U> && std::is_move_assignable_v<U>)>* = nullptr>
    void move_assign_from(fixed_vector&& /*other*/) {
        static_assert(std::is_move_constructible_v<U>,
                      "fixed_vector requires T to be move-constructible for move assignment.");
        static_assert(std::is_move_assignable_v<U>,
                      "fixed_vector requires T to be move-assignable for move assignment.");
    }

    /**
     * @brief Returns a pointer to the underlying storage.
     */
    constexpr pointer data() noexcept {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): needed for appropriate type
        return reinterpret_cast<pointer>(storage_.data());
    }

    /**
     * @brief Returns a const pointer to the underlying storage.
     */
    constexpr const_pointer data() const noexcept {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): needed for appropriate type
        return reinterpret_cast<const_pointer>(storage_.data());
    }
};

/**
 * @brief Checks if the contents of two fixed_vectors are equal.
 * @relates fixed_vector
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * @return true if the contents are equal, false otherwise.
 */
template <typename T, std::size_t N> bool operator==(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/**
 * @brief Checks if the contents of two fixed_vectors are not equal.
 * @relates fixed_vector
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * @return true if the contents are not equal, false otherwise.
 */
template <typename T, std::size_t N> bool operator!=(const fixed_vector<T, N>& lhs, const fixed_vector<T, N>& rhs) {
    return !(lhs == rhs);
}

} // namespace everest::lib::util
