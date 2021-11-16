#include <fsm/specialization/pthread.hpp>

#include "fsm.hpp"

void set_brightness(int value) {
    std::cout << "Set brightness to value: " << value << "\n";
}

int main(int argc, char* argv[]) {
    FSM fsm_desc;
    fsm::PThreadController<FSM::StateHandleType> fsm_ctrl;
    fsm_ctrl.run(fsm_desc.sd_light_off);
    
    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOn(), true);
    fsm_ctrl.submit_event(EventPressedOff(), true);
    fsm_ctrl.submit_event(EventPressedMot(), true);
    fsm_ctrl.submit_event(EventMotionDetect(), true);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    fsm_ctrl.stop();
    return 0;
}
