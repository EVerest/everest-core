// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdint>
#include <functional>

void timer_stub_stop_called(uint32_t called_count);
void timer_stub_timeout_called(uint32_t called_count);
void timer_stub_interval_called(uint32_t called_count);
void timer_stub_at_called(uint32_t called_count);
void timer_stub_set_callback(std::function<void()> callback_func);

void timer_stub_reset_stop_called_count();
void timer_stub_reset_timeout_called_count();
void timer_stub_reset_interval_called_count();
void timer_stub_reset_at_called_count();
void timer_stub_reset_callback();

uint32_t timer_stub_get_stop_called_count();
uint32_t timer_stub_get_timeout_called_count();
uint32_t timer_stub_get_interval_called_count();
uint32_t timer_stub_get_at_called_count();
std::function<void()> timer_stub_get_callback();
