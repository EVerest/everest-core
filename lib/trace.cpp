// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <backtrace-supported.h>
#include <backtrace.h>
#include <cxxabi.h>

#include <mutex>

static backtrace_state* bt_state;
static bool tried_to_initialize;
static std::mutex init_mtx;

struct StackTrace {
    std::string info;
};

inline int frame_handler(void* data, uintptr_t pc, const char* filename, int lineno, const char* function) {
    // FIXME(aw): we might have unknown frames everywhere in the call
    //            stack, it probably only make sense to skip the
    //            contigeous set of the top/root frames, that are all
    //            unknown
    // if ((filename == nullptr) && (lineno == 0) && (function == nullptr)) {
    //     return 1; // stop backtracing
    // }
    auto& trace = *static_cast<StackTrace*>(data);

    std::string function_name = "<function unknown>";
    if (function != nullptr) {
        // try to demangle
        int status;
        size_t length;

        auto demangled_function = __cxxabiv1::__cxa_demangle(function, nullptr, &length, &status);
        if (status == 0) {
            // successfully demangled
            function_name = std::string(demangled_function);
        } else {
            function_name = std::string(function);
        }
    }

    trace.info += (filename != nullptr) ? filename : "<filename unknown>";
    trace.info += (":" + std::to_string(lineno) + " @ ");
    trace.info += function_name;
    trace.info += "\n";

    return 0; // continue backtracing
}

namespace Everest {
namespace Logging {
std::string trace() {
    {
        std::lock_guard<std::mutex> lck(init_mtx);
        if (!tried_to_initialize) {
            // 1 means support threaded
            bt_state = backtrace_create_state(nullptr, 1, nullptr, nullptr);
            tried_to_initialize = true;
        }
    }

    if (bt_state == nullptr) {
        return "Backtrace functionality not available\n";
    }
    
    StackTrace trace;

    // FIXME (aw): 1 means we're skipping this functions frame, can this
    //             be optimized away by the compiler, so we are going to
    //             miss the first frame?
    backtrace_full(bt_state, 1, frame_handler, nullptr, &trace);

    return trace.info;
}
} // namespace Logging
} // namespace Everest
