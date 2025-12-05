// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <cstdint>
#include <functional>

#include "timer_stub.hpp"

static uint32_t stop_called_count;
static uint32_t timeout_called_count;
static uint32_t interval_called_count;
static uint32_t at_called_count;
static std::function<void()> callback;

void timer_stub_stop_called(uint32_t called_count) {
    stop_called_count += called_count;
}

void timer_stub_timeout_called(uint32_t called_count) {
    timeout_called_count += called_count;
}

void timer_stub_interval_called(uint32_t called_count) {
    interval_called_count += called_count;
}

void timer_stub_at_called(uint32_t called_count) {
    at_called_count += called_count;
}

void timer_stub_set_callback(std::function<void()> callback_func) {
    callback = callback_func;
}

void timer_stub_reset_stop_called_count() {
    stop_called_count = 0;
}

void timer_stub_reset_timeout_called_count() {
    timeout_called_count = 0;
}

void timer_stub_reset_interval_called_count() {
    interval_called_count = 0;
}

void timer_stub_reset_at_called_count() {
    at_called_count = 0;
}

void timer_stub_reset_callback() {
    callback = nullptr;
}

uint32_t timer_stub_get_stop_called_count() {
    return stop_called_count;
}

uint32_t timer_stub_get_timeout_called_count() {
    return timeout_called_count;
}

uint32_t timer_stub_get_interval_called_count() {
    return interval_called_count;
}

uint32_t timer_stub_get_at_called_count() {
    return at_called_count;
}

std::function<void()> timer_stub_get_callback() {
    return callback;
}
