// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef FSM_SPECIALIZATION_FREERTOS_HPP
#define FSM_SPECIALIZATION_FREERTOS_HPP

extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
}

#include <fsm/async.hpp>

namespace fsm {
namespace async {
namespace freertos {

template <typename ControllerType, typename EventBaseType>
class FSMContextCtrlImpl : public FSMContextCtrl<ControllerType, EventBaseType> {
public:
    FSMContextCtrlImpl(ControllerType& ctrl) : FSMContextCtrl<ControllerType, EventBaseType>(ctrl) {
        cv = xSemaphoreCreateBinaryStatic(&cv_buffer);
    }

    void wait() override final {
        if (cancelled) {
            return;
        }
        xSemaphoreTake(cv, portMAX_DELAY);
    }

    // wait_for should return true if timeout successful
    bool wait_for(int timeout_ms) override final {
        if (cancelled) {
            return false;
        }
        return xSemaphoreTake(cv, timeout_ms / portTICK_PERIOD_MS) == pdFALSE;
    }

    void reset() override final {
        xSemaphoreTake(cv, 0);
        cancelled = false;
    }

    void cancel() override final {
        cancelled = true;
        xSemaphoreGive(cv);
    }

    bool got_cancelled() override final {
        return cancelled;
    }

private:
    volatile bool cancelled{false};
    SemaphoreHandle_t cv;
    StaticSemaphore_t cv_buffer;
};

class PushPullLock {
public:
    class PullLock {
    public:
        PullLock(PushPullLock& pp_lock) : pp_lock(pp_lock) {
            // wait for push condition
            xSemaphoreTake(pp_lock.push_cv, portMAX_DELAY);

            // aquire mutex
            xSemaphoreTake(pp_lock.mutex, portMAX_DELAY);
        }

        ~PullLock() {
            // FIXME (aw): what is the correct/most performant order here?
            xSemaphoreGive(pp_lock.pull_cv);
            xSemaphoreGive(pp_lock.mutex);
        }

    private:
        PushPullLock& pp_lock;
    };

    class PushLock {
    public:
        PushLock(PushPullLock& pp_lock) : pp_lock(pp_lock) {
            // aquire mutex
            xSemaphoreTake(pp_lock.mutex, portMAX_DELAY);
        }

        void wait_for_pull() {
            xSemaphoreGive(pp_lock.mutex);
            xSemaphoreTake(pp_lock.pull_cv, portMAX_DELAY);
            xSemaphoreTake(pp_lock.mutex, portMAX_DELAY);
        }

        void commit() {
            mutex_released = true;
            xSemaphoreGive(pp_lock.mutex);

            // notify cv
            xSemaphoreGive(pp_lock.push_cv);
        }

        ~PushLock() {
            if (mutex_released) {
                return;
            }

            xSemaphoreGive(pp_lock.mutex);
        }

    private:
        PushPullLock& pp_lock;
        bool mutex_released{false};
    };

    PushPullLock() {
        mutex = xSemaphoreCreateMutexStatic(&mutex_buffer);
        push_cv = xSemaphoreCreateBinaryStatic(&push_cv_buffer);
        pull_cv = xSemaphoreCreateBinaryStatic(&pull_cv_buffer);
    }

    PullLock wait_for_pull_lock() {
        return PullLock(*this);
    }

    PushLock get_push_lock() {
        return PushLock(*this);
    }

private:
    SemaphoreHandle_t mutex;
    SemaphoreHandle_t push_cv;
    SemaphoreHandle_t pull_cv;
    StaticSemaphore_t mutex_buffer;
    StaticSemaphore_t push_cv_buffer;
    StaticSemaphore_t pull_cv_buffer;
};

template <int StackSize = 400, const char* Name = nullptr> class Thread {
public:
    Thread() {
        task_finished = xSemaphoreCreateBinaryStatic(&task_finished_buffer);
    }
    template <typename T, void (T::*mem_fn)(void)> bool spawn(T* class_inst) {
        if (task) {
            return false;
        }

        this->inst = class_inst;

        xTaskCreate(&Thread<StackSize, Name>::trampoline<T, mem_fn>, Name, StackSize, this, tskIDLE_PRIORITY, &task);

        return true;
    }

    bool join() {
        if (!task) {
            return false;
        }

        xSemaphoreTake(task_finished, portMAX_DELAY);
        task = nullptr;
        return true;
    }

private:
    template <typename T, void (T::*mem_fn)(void)> static void trampoline(void* pvParameters) {
        auto this_thread = static_cast<Thread<StackSize, Name>*>(pvParameters);
        auto class_instance = static_cast<T*>(this_thread->inst);

        // run the function
        (class_instance->*mem_fn)();

        // when we come here, the original function exited, so we can notify and delete ourself
        xSemaphoreGive(this_thread->task_finished);

        vTaskDelete(nullptr);
    }

    void* inst{nullptr};
    TaskHandle_t task{nullptr};
    SemaphoreHandle_t task_finished;
    StaticSemaphore_t task_finished_buffer;
};
} // namespace freertos

template <typename StateHandleType, int StackSize = 400, const char* Name = nullptr,
          void (*LogFunction)(const std::string&) = nullptr>
using FreeRTOSController = BasicController<StateHandleType, freertos::PushPullLock, freertos::Thread<StackSize, Name>,
                                           freertos::FSMContextCtrlImpl, LogFunction>;

} // namespace async
} // namespace fsm

#endif // FSM_SPECIALIZATION_FREERTOS_HPP
