// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef FSM_UTILS_STATIC_RING_BUFFER_HPP
#define FSM_UTILS_STATIC_RING_BUFFER_HPP

#include <memory>
#include <type_traits>

namespace fsm {
namespace utils {

template <typename BaseType, size_t SIZE> class StaticRingBuffer {
public:
    BaseType& peek() {
        const HeaderType& cur_header_info = *reinterpret_cast<HeaderType*>(tail);

        size_t offset_to_payload = (tail - buffer + cur_header_info.offset) % SIZE;

        return *reinterpret_cast<BaseType*>(buffer + offset_to_payload);
    }

    bool empty() {
        return (tail == head && !full);
    }

    bool pop() {
        if (tail == head && !full) {
            return false;
        }

        const HeaderType& cur_header_info = *reinterpret_cast<HeaderType*>(tail);

        tail = buffer + cur_header_info.next;

        full = false;

        return true;
    }

    template <typename T> bool push(const T& obj) {
        void* dst = alloc_next(sizeof(T), alignof(T));

        if (dst) {
            new (dst) T(obj);
            return true;
        }
        return false;
    }

private:
    using NextType = uint16_t;
    using PaddingType = uint8_t;
    struct HeaderType {
        NextType next;
        PaddingType offset;
    };
    constexpr static size_t HEADER_SIZE = sizeof(HeaderType);

    void* alloc_next(size_t size, size_t alignment) {
        if (full) {
            return nullptr;
        }

        auto dst = reinterpret_cast<unsigned char*>(get_payload_ptr(size, alignment));
        if (!dst) {
            return nullptr;
        }

        // space found, setup header struct
        // first: setup offset to dst buffer
        HeaderType& cur_header_info = *reinterpret_cast<HeaderType*>(head);
        if (dst > head) {
            cur_header_info.offset = dst - head;
        } else {
            // handle wrap around
            // FIXME (aw): verify this formular!
            cur_header_info.offset = SIZE - (head - dst);
        }

        // second: find next slot for header struct
        void* next_hdr_ptr = dst + size;
        size_t space;
        if (dst < tail) {
            // either | .. head .. dst .. tail .. |
            // or     | .. dst .. tail .. head .. |
            // in both cases, the next header struct has to fit into the
            // buffer, because there is still space for the tail header struct
            space = 2 * HEADER_SIZE;
            std::align(alignof(HeaderType), HEADER_SIZE, next_hdr_ptr, space);
        } else {
            // | .. tail .. head .. dst .. |
            // check if next header fits into the end
            space = SIZE - (reinterpret_cast<unsigned char*>(next_hdr_ptr) - buffer);
            if (!std::align(alignof(HeaderType), HEADER_SIZE, next_hdr_ptr, space)) {
                next_hdr_ptr = buffer;
            }
        }

        const auto next_head = reinterpret_cast<unsigned char*>(next_hdr_ptr);
        cur_header_info.next = next_head - buffer;

        head = next_head;

        if (head == tail) {
            full = true;
        }

        return dst;
    }

    void* get_payload_ptr(size_t size, size_t alignment) {

        // head points to next valid header struct
        size_t space_left;
        void* space_ptr = head + HEADER_SIZE;
        ;

        if (head < tail) {
            // head wrapped around and tail not
            // left space only between head and tail
            space_left = tail - head - HEADER_SIZE;

            return std::align(alignment, size, space_ptr, space_left);
        }

        // head => tail
        // first check space between head and end boundary
        space_left = SIZE - (head - buffer);
        if (std::align(alignment, size, space_ptr, space_left)) {
            return space_ptr;
        }

        // it doesn't fit, so
        // second check space between beginning and tail
        space_left = tail - buffer;
        space_ptr = buffer;

        return std::align(alignment, size, space_ptr, space_left);
    }

    alignas(HeaderType) unsigned char buffer[SIZE];
    // FIXME (aw): static_assert SIZE > HEADER_SIZE

    bool full{false};

    unsigned char* head{buffer};
    unsigned char* tail{buffer};
};

} // namespace utils
} // namespace fsm

#endif // FSM_UTILS_STATIC_RING_BUFFER_HPP
