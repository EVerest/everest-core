// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef FSM_SPECIALIZATION_PTHREAD_HPP
#define FSM_SPECIALIZATION_PTHREAD_HPP

#include <fsm/async.hpp>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace fsm {
namespace async {

namespace pthread {

template <typename ControllerType, typename EventBaseType>
class FSMContextCtrlImpl : public FSMContextCtrl<ControllerType, EventBaseType> {
public:
    using FSMContextCtrl<ControllerType, EventBaseType>::FSMContextCtrl;

    void wait() override final {
        std::unique_lock<std::mutex> lck(mutex);

        cv.wait(lck, [this]() { return cancelled; });
    }

    // wait_for should return true if timeout successful
    bool wait_for(int timeout_ms) override final {
        std::unique_lock<std::mutex> lck(mutex);

        return !cv.wait_for(lck, std::chrono::milliseconds(timeout_ms), [this]() { return cancelled; });
    }

    void reset() override final {
        std::lock_guard<std::mutex> lck(mutex);
        cancelled = false;
    }

    void cancel() override final {
        {
            std::lock_guard<std::mutex> lck(mutex);
            cancelled = true;
        }
        cv.notify_one();
    }

    bool got_cancelled() override final {
        std::lock_guard<std::mutex> lck(mutex);
        return cancelled;
    }

private:
    std::mutex mutex;
    bool cancelled{false};
    std::condition_variable cv;
};

class PushPullLock {
public:
    class PullLock {
    public:
        PullLock(PushPullLock& pp_lock) : lck(pp_lock.mutex), pp_lock(pp_lock) {
            pp_lock.cv.wait(lck, [&pp_lock]() { return pp_lock.pushed; });
        }

        ~PullLock() {
            pp_lock.pushed = false;
            pp_lock.cv.notify_all();
        }

        PullLock(PullLock&&) = default;

    private:
        std::unique_lock<std::mutex> lck;
        PushPullLock& pp_lock;
    };

    class PushLock {
    public:
        PushLock(PushPullLock& pp_lock) : lck(pp_lock.mutex), pp_lock(pp_lock){};

        void wait_for_pull() {
            pp_lock.cv.wait(lck, [this]() { return !pp_lock.pushed; });
        }

        void commit() {
            pp_lock.pushed = true;
            lck.unlock();
            pp_lock.cv.notify_all();
        }

    private:
        std::unique_lock<std::mutex> lck;
        PushPullLock& pp_lock;
    };

    PushPullLock() {
    }

    PullLock wait_for_pull_lock() {
        return PullLock(*this);
    }

    PushLock get_push_lock() {
        return PushLock(*this);
    }

private:
    std::mutex mutex;
    std::condition_variable cv;
    bool pushed{false};
};

class Thread {
public:
    Thread() {
    }
    template <typename T, void (T::*mem_fn)(void)> bool spawn(T* class_inst) {
        if (spawned) {
            return false;
        }

        thread = std::thread(mem_fn, class_inst);

        spawned = true;
        return true;
    }

    bool join() {
        if (!spawned) {
            return false;
        }

        thread.join();

        spawned = false;
        return true;
    }

private:
    bool spawned{false};
    std::thread thread;
};

} // namespace pthread

template <typename StateHandleType, void (*LogFunction)(const std::string&) = nullptr>
using PThreadController =
    BasicController<StateHandleType, pthread::PushPullLock, pthread::Thread, pthread::FSMContextCtrlImpl, LogFunction>;

} // namespace async
} // namespace fsm

#endif // FSM_SPECIALIZATION_PTHREAD_HPP
