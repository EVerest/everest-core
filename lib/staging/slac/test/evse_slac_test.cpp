// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <chrono>
#include <cstdio>
#include <optional>
#include <thread>

#include <slac/fsm/evse/fsm.hpp>
#include <slac/fsm/evse/states/others.hpp>

#include <fmt/format.h>

static auto create_cm_set_key_cnf() {
    // FIXME (aw): needs to be fully implemented!
    slac::messages::cm_set_key_cnf set_key_cnf;
    slac::messages::HomeplugMessage hp_message;
    hp_message.setup_payload(&set_key_cnf, sizeof(set_key_cnf),
                             (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF), slac::defs::MMV::AV_1_1);
    return hp_message;
}

static auto create_cm_validate_req() {
    slac::messages::cm_validate_req validate_req;
    validate_req.signal_type = slac::defs::CM_VALIDATE_REQ_SIGNAL_TYPE;
    validate_req.timer = 0;
    validate_req.result = slac::defs::CM_VALIDATE_REQ_RESULT_READY;

    slac::messages::HomeplugMessage hp_message;
    hp_message.setup_payload(&validate_req, sizeof(validate_req),
                             (slac::defs::MMTYPE_CM_VALIDATE | slac::defs::MMTYPE_MODE_REQ), slac::defs::MMV::AV_1_1);

    return hp_message;
}

struct EVSession {
    EVSession(const std::array<uint8_t, 8>& run_id_, const std::array<uint8_t, 6>& mac_) : run_id(run_id_), mac(mac_){};

    // FIXME (aw): all these create_cm_* need to be fully implemented!
    auto create_cm_slac_parm_req() {
        slac::messages::cm_slac_parm_req parm_req;
        std::copy(run_id.begin(), run_id.end(), parm_req.run_id);

        slac::messages::HomeplugMessage hp_message;
        hp_message.setup_ethernet_header(mac.data(), mac.data());
        hp_message.setup_payload(&parm_req, sizeof(parm_req),
                                 (slac::defs::MMTYPE_CM_SLAC_PARAM | slac::defs::MMTYPE_MODE_REQ),
                                 slac::defs::MMV::AV_1_1);

        return hp_message;
    }

    auto create_cm_start_atten_char_ind() {
        slac::messages::cm_start_atten_char_ind atten_char_ind;
        std::copy(run_id.begin(), run_id.end(), atten_char_ind.run_id);

        slac::messages::HomeplugMessage hp_message;
        hp_message.setup_ethernet_header(mac.data(), mac.data());
        hp_message.setup_payload(&atten_char_ind, sizeof(atten_char_ind),
                                 (slac::defs::MMTYPE_CM_START_ATTEN_CHAR | slac::defs::MMTYPE_MODE_IND),
                                 slac::defs::MMV::AV_1_1);

        return hp_message;
    }

    auto create_cm_mnbc_sound_ind() {
        slac::messages::cm_mnbc_sound_ind sound_ind;
        std::copy(run_id.begin(), run_id.end(), sound_ind.run_id);

        slac::messages::HomeplugMessage hp_message;
        hp_message.setup_ethernet_header(mac.data(), mac.data());
        hp_message.setup_payload(&sound_ind, sizeof(sound_ind),
                                 (slac::defs::MMTYPE_CM_MNBC_SOUND | slac::defs::MMTYPE_MODE_IND),
                                 slac::defs::MMV::AV_1_1);

        return hp_message;
    }

    auto create_cm_atten_profile_ind() {
        slac::messages::cm_atten_profile_ind profile_ind;
        std::copy(mac.begin(), mac.end(), profile_ind.pev_mac);
        profile_ind.num_groups = slac::defs::AAG_LIST_LEN;

        for (int i = 0; i < slac::defs::AAG_LIST_LEN; ++i) {
            profile_ind.aag[i] = i;
        }

        slac::messages::HomeplugMessage hp_message;
        hp_message.setup_ethernet_header(mac.data(), mac.data());
        hp_message.setup_payload(&profile_ind, sizeof(profile_ind),
                                 (slac::defs::MMTYPE_CM_ATTEN_PROFILE | slac::defs::MMTYPE_MODE_IND),
                                 slac::defs::MMV::AV_1_1);

        return hp_message;
    }

    auto create_cm_atten_char_rsp() {
        slac::messages::cm_atten_char_rsp atten_char;
        std::copy(run_id.begin(), run_id.end(), atten_char.run_id);

        slac::messages::HomeplugMessage hp_message;
        hp_message.setup_ethernet_header(mac.data(), mac.data());
        hp_message.setup_payload(&atten_char, sizeof(atten_char),
                                 (slac::defs::MMTYPE_CM_ATTEN_CHAR | slac::defs::MMTYPE_MODE_RSP),
                                 slac::defs::MMV::AV_1_1);

        return hp_message;
    }

    auto create_cm_slac_match_req() {
        slac::messages::cm_slac_match_req match_req;
        std::copy(run_id.begin(), run_id.end(), match_req.run_id);

        slac::messages::HomeplugMessage hp_message;
        hp_message.setup_ethernet_header(mac.data(), mac.data());
        hp_message.setup_payload(&match_req, sizeof(match_req),
                                 (slac::defs::MMTYPE_CM_SLAC_MATCH | slac::defs::MMTYPE_MODE_REQ),
                                 slac::defs::MMV::AV_1_1);

        return hp_message;
    }

private:
    std::array<uint8_t, 8> run_id;
    const std::array<uint8_t, ETH_ALEN>& mac;
};

void feed_machine_for(slac::fsm::evse::FSM& machine, int period_ms,
                      fsm::FeedResult<slac::fsm::evse::FSMReturnType> feed_result) {
    using namespace std::chrono;

    auto end_tp = steady_clock::now() + milliseconds(period_ms);

    while (true) {
        if (feed_result.transition()) {
            // need to call feed
        } else if (feed_result.unhandled_event()) {
            printf("DEBUG: got an unhandled event\n");
            break;
        } else if (feed_result.internal_error()) {
            printf("ERROR: internal FSM error\n");
            exit(EXIT_FAILURE);
        } else if (feed_result.has_value() == false) {
            // nothing to do
            break;
        } else if (feed_result.has_value() == true) {
            const auto timeout = *feed_result;
            if (timeout == 0) {
                // need to call feed directly
            } else {
                auto next_tp = steady_clock::now() + milliseconds(timeout);
                if (next_tp > end_tp) {
                    break;
                }

                std::this_thread::sleep_until(next_tp);
            }
        } else {
            printf("ERROR: unknown feed result case\n");
            exit(EXIT_FAILURE);
        }

        feed_result = machine.feed();
    }

    std::this_thread::sleep_until(end_tp);
}

int main(int argc, char* argv[]) {
    const auto ATTENUATION_ADJUSTMENT = 10;
    printf("Hi from SLAC!\n");

    std::optional<slac::messages::HomeplugMessage> msg_in;

    slac::fsm::evse::ContextCallbacks callbacks;
    callbacks.log = [](const std::string& text) { fmt::print("SLAC LOG: {}\n", text); };

    callbacks.send_raw_slac = [&msg_in](slac::messages::HomeplugMessage& hp_message) { msg_in = hp_message; };

    auto ctx = slac::fsm::evse::Context(callbacks);
    ctx.slac_config.sounding_atten_adjustment = ATTENUATION_ADJUSTMENT;
    ctx.slac_config.chip_reset.enabled = false;

    auto machine = slac::fsm::evse::FSM();

    //
    // reset machine
    //
    machine.reset<slac::fsm::evse::ResetState>(ctx);
    auto fr = machine.feed();

    // assert that CM_SET_KEY_REQ gets set!
    if (!msg_in.has_value() || msg_in->get_mmtype() != (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_REQ)) {
        printf("Expected CM_SET_KEY_REQ!\n");
        exit(EXIT_FAILURE);
    } else {
        msg_in.reset();
    }

    feed_machine_for(machine, 230, fr);

    // feed in CM_SET_KEY_CNF
    ctx.slac_message_payload = create_cm_set_key_cnf();
    machine.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
    fr = machine.feed();

    // should be idle state now, send ENTER_BCD, to enter MATCHING
    machine.handle_event(slac::fsm::evse::Event::ENTER_BCD);
    fr = machine.feed();

    feed_machine_for(machine, 300, fr);

    // create session 1 and inject CM_SLAC_PARM_REQ
    auto session_1 = EVSession({0, 1, 2, 3, 4, 5, 6, 7}, {0xca, 0xfe, 0xca, 0xfe, 0xca, 0xfe});
    ctx.slac_message_payload = session_1.create_cm_slac_parm_req();
    machine.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
    fr = machine.feed();

    // assert that CM_SLAC_PARM_CNF gets set!
    if (!msg_in.has_value() ||
        msg_in->get_mmtype() != (slac::defs::MMTYPE_CM_SLAC_PARAM | slac::defs::MMTYPE_MODE_CNF)) {
        printf("Expected CM_SLAC_PARM_CNF!\n");
        exit(EXIT_FAILURE);
    } else {
        msg_in.reset();
    }

    feed_machine_for(machine, 233, fr);

    // inject CM_START_ATTEN_CHAR_IND
    ctx.slac_message_payload = session_1.create_cm_start_atten_char_ind();
    machine.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
    fr = machine.feed();

    // inject all the soundings ...
    for (int i = 0; i < slac::defs::CM_SLAC_PARM_CNF_NUM_SOUNDS - 1; i++) {
        ctx.slac_message_payload = session_1.create_cm_mnbc_sound_ind();
        machine.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
        fr = machine.feed();

        ctx.slac_message_payload = session_1.create_cm_atten_profile_ind();
        machine.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
        fr = machine.feed();
    }

    feed_machine_for(machine, 700 + 1 * 45, fr);

    // assert that CM_ATTEN_CHAR_IND gets set!
    if (!msg_in.has_value() ||
        msg_in->get_mmtype() != (slac::defs::MMTYPE_CM_ATTEN_CHAR | slac::defs::MMTYPE_MODE_IND)) {
        printf("Expected CM_ATTEN_CHAR_IND!\n");
        exit(EXIT_FAILURE);
    } else {
        auto atten_char_ind = msg_in->get_payload<slac::messages::cm_atten_char_ind>();
        for (int i = 0; i < slac::defs::AAG_LIST_LEN; ++i) {
            if (atten_char_ind.attenuation_profile.aag[i] != i + ATTENUATION_ADJUSTMENT) {
                printf("Averaging not correct in ATTEN_CHAR_IND\n");
                exit(EXIT_FAILURE);
            }
        }
        msg_in.reset();
    }

    // "async" insert an CM_VALIDATE.REQ
    ctx.slac_message_payload = create_cm_validate_req();
    machine.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
    fr = machine.feed();

    // assert that CM_VALIDATE.CNF gets set!
    if (!msg_in.has_value() || msg_in->get_mmtype() != (slac::defs::MMTYPE_CM_VALIDATE | slac::defs::MMTYPE_MODE_CNF)) {
        printf("Expected CM_VALIDATE.CNF!\n");
        exit(EXIT_FAILURE);
    } else {
        // check for correct "failure" result
        auto validate_cnf = msg_in->get_payload<slac::messages::cm_validate_cnf>();
        if (validate_cnf.result != slac::defs::CM_VALIDATE_REQ_RESULT_FAILURE) {
            printf("Expected result field of CM_VALIDATE.CNF to be set to failure\n");
            exit(EXIT_FAILURE);
        }
    }

    // inject CM_ATTEN_CHAR_RSP
    ctx.slac_message_payload = session_1.create_cm_atten_char_rsp();
    machine.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
    fr = machine.feed();

    feed_machine_for(machine, 1000, fr);

    // inject messages from a second session
    auto session_2 = EVSession({9, 1, 2, 3, 4, 5, 6, 7}, {0xbe, 0xaf, 0xbe, 0xaf, 0xbe, 0xaf});
    ctx.slac_message_payload = session_2.create_cm_slac_parm_req();
    machine.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
    fr = machine.feed();

    feed_machine_for(machine, 1000, fr);

    // inject CM_SLAC_MATCH_REQ
    ctx.slac_message_payload = session_1.create_cm_slac_match_req();
    machine.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
    fr = machine.feed();

    // assert that CM_SLAC_MATCH_CNF gets set!
    if (!msg_in.has_value() ||
        msg_in->get_mmtype() != (slac::defs::MMTYPE_CM_SLAC_MATCH | slac::defs::MMTYPE_MODE_CNF)) {
        printf("Expected CM_ATTEN_CHAR_IND!\n");
        exit(EXIT_FAILURE);
    } else {
        msg_in.reset();
    }

    return 0;
}
