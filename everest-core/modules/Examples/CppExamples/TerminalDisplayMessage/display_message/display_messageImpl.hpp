// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DISPLAY_MESSAGE_DISPLAY_MESSAGE_IMPL_HPP
#define DISPLAY_MESSAGE_DISPLAY_MESSAGE_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/display_message/Implementation.hpp>

#include "../TerminalDisplayMessage.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace display_message {

struct Conf {};

class display_messageImpl : public display_messageImplBase {
public:
    display_messageImpl() = delete;
    display_messageImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<TerminalDisplayMessage>& mod,
                        Conf& config) :
        display_messageImplBase(ev, "display_message"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::display_message::SetDisplayMessageResponse
    handle_set_display_message(std::vector<types::display_message::DisplayMessage>& request) override;
    virtual types::display_message::GetDisplayMessageResponse
    handle_get_display_messages(types::display_message::GetDisplayMessageRequest& request) override;
    virtual types::display_message::ClearDisplayMessageResponse
    handle_clear_display_message(types::display_message::ClearDisplayMessageRequest& request) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<TerminalDisplayMessage>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace display_message
} // namespace module

#endif // DISPLAY_MESSAGE_DISPLAY_MESSAGE_IMPL_HPP
