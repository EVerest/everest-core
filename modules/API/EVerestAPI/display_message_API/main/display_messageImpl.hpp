// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MAIN_DISPLAY_MESSAGE_IMPL_HPP
#define MAIN_DISPLAY_MESSAGE_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/display_message/Implementation.hpp>

#include "../display_message_API.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class display_messageImpl : public display_messageImplBase {
public:
    display_messageImpl() = delete;
    display_messageImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<display_message_API>& mod,
                        Conf& config) :
        display_messageImplBase(ev, "main"), mod(mod), config(config){};

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
    const Everest::PtrContainer<display_message_API>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here

    template <class T, class ReqT>
    auto generic_request_reply(T const& default_value, ReqT const& request, std::string const& topic);

    int timeout_s{5};

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_DISPLAY_MESSAGE_IMPL_HPP
