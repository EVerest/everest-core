// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

/**
 * \file sdbus-c++ version independent interfaces
 *
 * The sdbus-c++ library is used to provide a C++ interface to DBus.
 * The functions defined here enable code to compile and run against
 * version 1.4 or higher of sdbus-c++.
 */

#pragma once

#include <sdbus-c++/sdbus-c++.h>

namespace everest::lib::system::dbus {

using async_method_callback = std::function<void(sdbus::MethodReply reply, std::optional<sdbus::Error> error)>;
using async_property_callback = std::function<void(std::optional<sdbus::Error> error, sdbus::Variant value)>;

// library version independent functions
[[nodiscard]] std::unique_ptr<sdbus::IProxy> createProxy(const char* destination, const char* objectPath);
[[nodiscard]] std::unique_ptr<sdbus::IProxy> createProxy(const char* destination, const char* objectPath,
                                                         sdbus::dont_run_event_loop_thread_t);
[[nodiscard]] sdbus::MethodCall createMethodCall(const std::unique_ptr<sdbus::IProxy>& proxy, const char* interfaceName,
                                                 const char* methodName);
sdbus::PendingAsyncCall callMethodAsync(const std::unique_ptr<sdbus::IProxy>& proxy, sdbus::MethodCall& method,
                                        async_method_callback handler, uint64_t timeout_us);
sdbus::PendingAsyncCall getPropertyAsync(const std::unique_ptr<sdbus::IProxy>& proxy, async_property_callback handler,
                                         const char* property, const char* interface);
void registerSignalHandler(std::unique_ptr<sdbus::IProxy>& proxy, const char* interfaceName, const char* signalName,
                           sdbus::signal_handler signalHandler);

void initialise_handlers(const std::unique_ptr<sdbus::IProxy>& proxy, std::function<void(void)> init);
void process_pending(sdbus::IConnection& connection);

} // namespace everest::lib::system::dbus
