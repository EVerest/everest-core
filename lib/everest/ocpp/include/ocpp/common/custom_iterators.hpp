// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <memory>
#include <vector>

namespace ocpp {

// This file contains the implementation of a couple of custom iterators, enabling the iteration and abstraction of more
// complex containers. For example we want to be able to iterate a std::vector<std::unique_ptr<T>> and get references to
// T from the iterator
//
// ForwardIterator<T> implements a custom iterator for type T that does not depend on any underlying container.
//   You can add this for your type by returning ForwardIterator<T> from begin() and end()
// ForwardIterator can only be constructed by passing a ForwardIteratorBase derived struct in a unique_ptr.
//   ForwardIteratorBase is an abstract class that allows us to implement container specific functionality.
// VectorOfUniquePtrIterator is an implementation of the ForwardIteratorBase specifically catered to the example of
// std::vector<std:unique_ptr<T>>
//   So to implement the begin() and end() functions for your type, you could do the following:
//   std::vector<std::unique_ptr<T>> container;
//   ForwardIterator<T> begin() {
//       return ForwardIterator<T>(std::make_unique<VectorOfUniquePtrIterator<T>>(container.begin()));
//   }

/// \brief Abstract struct to implement to enable the use of the ForwardIterator<T>
template <typename T> struct ForwardIteratorBase {
    virtual ~ForwardIteratorBase() = default;

    /// \brief Get a reference to the value pointed to by the iterator
    virtual T& deref() const = 0;
    /// \brief Increment the iterator once to the next position
    virtual void advance() = 0;
    /// \brief Check for equality between this iterator and \p other
    virtual bool not_equal(const ForwardIteratorBase& other) = 0;
};

/// \brief Helper struct that allows the use of an iterator in an interface, can be implemented using any forward
/// iterator
template <typename T> struct ForwardIterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using base = ForwardIteratorBase<T>;

    /// \brief Construct a new wrapper using any type that implements the abstract struct Base
    explicit ForwardIterator(std::unique_ptr<base> it) : it{std::move(it)} {
    }

    /// \brief Get a reference to the value pointed to by the iterator
    reference operator*() const {
        return it->deref();
    };

    /// \brief Increment the iterator once to the next position
    ForwardIterator& operator++() {
        it->advance();
        return *this;
    }

    /// \brief Check for inequality between this \p a and \p b
    friend bool operator!=(const ForwardIterator& a, const ForwardIterator& b) {
        return a.it->not_equal(*b.it);
    };

private:
    std::unique_ptr<base> it;
};

/// \brief Implementation for the ForwardIteratorBase based on a vector of unique_ptr with type T
template <typename T> struct VectorOfUniquePtrIterator : ForwardIteratorBase<T> {
    using iterator_type = typename std::vector<std::unique_ptr<T>>::iterator;
    using base = ForwardIteratorBase<T>;

    explicit VectorOfUniquePtrIterator(iterator_type it) : it{it} {
    }

    T& deref() const override {
        return *it->get();
    }

    void advance() override {
        ++it;
    }

    bool not_equal(const base& other) override {
        return it != dynamic_cast<const VectorOfUniquePtrIterator&>(other).it;
    }

private:
    iterator_type it;
};

} // namespace ocpp