// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/**
 * @file Thread pool
 * @brief info
 */

#pragma once

#include <everest/util/queue/thread_safe_queue.hpp>
#include <functional>
#include <future>
#include <thread>

namespace everest::lib::util{

class thread_pool {
public:
    using action = std::function<void()>;
    thread_pool(unsigned int thread_count) {
        auto action = [this] {
            while (auto task = m_action_queue.wait_and_pop()) {
                // Task successful, execute it while handling exceptions
                try {
                    task.value()();
                } catch (...) {
                    // Log the error if possible, but keep the thread alive.
                }
            }
        };
        for (size_t i = 0; i < thread_count; ++i) {
            m_threads.emplace_back(action);
        }
    }

    ~thread_pool() {
        m_action_queue.stop();
        for (auto& elem : m_threads) {
            elem.join();
        }
    }

    template <typename F, typename... Args>
    auto operator()(F&& f, Args&&... args) const -> std::future<std::invoke_result_t<F, Args...>> {
        using R = std::invoke_result_t<F, Args...>;

        auto prom = std::make_shared<std::promise<R>>();
        auto fut = prom->get_future();
        enqueue_task(std::forward<F>(f), prom, std::forward<Args>(args)...);

        return fut;
    }

    template <typename F, typename... Args> void run(F&& f, Args&&... args) const {
        this->operator()(std::forward<F>(f), std::forward<Args>(args)...);
    }

private:
    template <typename Fut, typename F> static void promise_set_value(std::promise<Fut>& prom, F& f) {
        prom.set_value(f());
    }

    template <typename F> static void promise_set_value(std::promise<void>& prom, F& f) {
        f();
        prom.set_value();
    }

    template <typename F, typename... Args, typename R>
    void enqueue_task(F&& f, std::shared_ptr<std::promise<R>>& prom, Args&&... args) const {

        auto bound_f = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        m_action_queue.push([prom, task_f = std::move(bound_f)]() mutable {
            try {
                promise_set_value(*prom, task_f);
            } catch (std::exception&) {
                prom->set_exception(std::current_exception());
            }
        });
    }
    mutable thread_safe_queue<action> m_action_queue;
    std::vector<std::thread> m_threads;
};

namespace everest::lib::util {}
} // namespace everest::lib::util::testing_interface
