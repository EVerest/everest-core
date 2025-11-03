// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>

// FIXME (aw): we only need the payload types from sdp.hpp, this could be shared in a separate header file
#include <iso15118/io/sdp.hpp>
#include <iso15118/io/stream_view.hpp>

#include "type.hpp"

namespace iso15118::message_20 {

class Variant {
public:
    using CustomDeleter = void (*)(void*);
    Variant(io::v2gtp::PayloadType, const io::StreamInputView&);
    template <typename MessageType> Variant(const MessageType& in) {
        static_assert(TypeTrait<MessageType>::type != Type::None, "Unhandled type!");

        data = new MessageType;
        *static_cast<MessageType*>(data) = in;
        custom_deleter = [](void* ptr) { delete static_cast<MessageType*>(ptr); };
        type = message_20::TypeTrait<MessageType>::type;
    }
    ~Variant();

    Type get_type() const;

    const std::string& get_error() const;

    template <typename T> const T& get() const {
        static_assert(TypeTrait<T>::type != Type::None, "Unhandled type!");
        if (TypeTrait<T>::type != type) {
            throw std::runtime_error("Illegal message type access");
        }

        return *static_cast<T*>(data);
    }

    template <typename T> T const* get_if() const {
        static_assert(TypeTrait<T>::type != Type::None, "Unhandled type!");
        if (TypeTrait<T>::type != type) {
            return nullptr;
        }

        return static_cast<T*>(data);
    }

private:
    CustomDeleter custom_deleter{nullptr};
    void* data{nullptr};
    Type type{Type::None};
    std::string error;
};
} // namespace iso15118::message_20
