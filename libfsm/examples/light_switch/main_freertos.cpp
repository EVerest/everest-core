#include <stdlib.h>

extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
}

#include <fsm/specialization/freertos.hpp>

#include "fsm.hpp"

constexpr int STACK_SIZE = 400;

void log(const std::string& msg) {
    vLoggingPrintf("fsm log: %s\n", msg.c_str());
}

void delay(int ms) {
    constexpr TickType_t delay = 1000 / portTICK_PERIOD_MS;
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

void set_brightness(int value) {
    vLoggingPrintf("Set brightness to value: %d\n", value);
}

void main_task(void* pvParameters) {
    FSM fsm_desc;
    fsm::FreeRTOSController<FSM::StateHandleType> fsm_ctrl;
    fsm_ctrl.run(fsm_desc.sd_light_off);

    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOff(), true);
    fsm_ctrl.submit_event(EventPressedMot(), true);
    fsm_ctrl.submit_event(EventMotionDetect(), true);
    delay(5000);

    fsm_ctrl.stop();

    exit(0);

    // vTaskDelete(nullptr);
}

int main(int argc, char* argv[]) {
    xTaskCreate(main_task, "main_task", STACK_SIZE, nullptr, tskIDLE_PRIORITY, nullptr);

    vTaskStartScheduler();

    return 0;
}
