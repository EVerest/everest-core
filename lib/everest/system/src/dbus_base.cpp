// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/system/dbus_base.hpp>

namespace everest::lib::system::dbus {

void initialise_handlers(const std::unique_ptr<sdbus::IProxy>& proxy, std::function<void(void)> init) {
    init();
#if SDBUSCPP_MAJOR_VERSION == 1
    proxy->finishRegistration();
#endif
}

void process_pending(sdbus::IConnection& connection) {
    for (;;) {
#if SDBUSCPP_MAJOR_VERSION == 1
        if (!connection.processPendingRequest()) {
            break;
        }
#else
        if (!connection.processPendingEvent()) {
            break;
        }
#endif
    }
}

std::unique_ptr<sdbus::IProxy> createProxy(const char* destination, const char* objectPath) {
    std::unique_ptr<sdbus::IProxy> result;
#if SDBUSCPP_MAJOR_VERSION == 1
    result = sdbus::createProxy(destination, objectPath);
#else
    auto sname = sdbus::ServiceName{destination};
    auto opath = sdbus::ObjectPath{objectPath};
    result = sdbus::createProxy(sname, opath);
#endif
    return result;
}

std::unique_ptr<sdbus::IProxy> createProxy(const char* destination, const char* objectPath,
                                           sdbus::dont_run_event_loop_thread_t) {
    std::unique_ptr<sdbus::IProxy> result;
#if SDBUSCPP_MAJOR_VERSION == 1
    result = sdbus::createProxy(destination, objectPath, sdbus::dont_run_event_loop_thread);
#else
    auto sname = sdbus::ServiceName{destination};
    auto opath = sdbus::ObjectPath{objectPath};
    result = sdbus::createProxy(sname, opath, sdbus::dont_run_event_loop_thread);
#endif
    return result;
}

sdbus::MethodCall createMethodCall(const std::unique_ptr<sdbus::IProxy>& proxy, const char* interfaceName,
                                   const char* methodName) {
    sdbus::MethodCall result;
#if SDBUSCPP_MAJOR_VERSION == 1
    result = proxy->createMethodCall(interfaceName, methodName);
#else
    auto iname = sdbus::InterfaceName{interfaceName};
    auto mname = sdbus::MethodName{methodName};
    result = proxy->createMethodCall(iname, mname);
#endif
    return result;
}

sdbus::PendingAsyncCall callMethodAsync(const std::unique_ptr<sdbus::IProxy>& proxy, sdbus::MethodCall& method,
                                        async_method_callback handler, uint64_t timeout_us) {
    sdbus::PendingAsyncCall result;
#if SDBUSCPP_MAJOR_VERSION == 1
    auto invoke = [handler](sdbus::MethodReply reply, const sdbus::Error* error) {
        std::optional<sdbus::Error> err;
        if (error != nullptr) {
            err = *error;
        }
        handler(reply, err);
    };
    result = proxy->callMethod(method, invoke, timeout_us);
#else
    result = proxy->callMethodAsync(method, handler, timeout_us);
#endif
    return result;
}

sdbus::PendingAsyncCall getPropertyAsync(const std::unique_ptr<sdbus::IProxy>& proxy, async_property_callback handler,
                                         const char* property, const char* interface) {
    sdbus::PendingAsyncCall result;
#if SDBUSCPP_MAJOR_VERSION == 1
    auto invoke = [handler](const sdbus::Error* error, sdbus::Variant value) {
        std::optional<sdbus::Error> err;
        if (error != nullptr) {
            err = *error;
        }
        handler(err, value);
    };
#else
    auto& invoke = handler;
#endif
    return proxy->getPropertyAsync(property).onInterface(interface).uponReplyInvoke(invoke);
}

void registerSignalHandler(std::unique_ptr<sdbus::IProxy>& proxy, const char* interfaceName, const char* signalName,
                           sdbus::signal_handler signalHandler) {
#if SDBUSCPP_MAJOR_VERSION == 1
    proxy->registerSignalHandler(interfaceName, signalName, signalHandler);
#else
    auto iname = sdbus::InterfaceName{interfaceName};
    auto sname = sdbus::SignalName{signalName};
    proxy->registerSignalHandler(iname, sname, signalHandler);
#endif
}

} // namespace everest::lib::system::dbus
