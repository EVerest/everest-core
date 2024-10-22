#include <cstdio>
#include <stdexcept>

#include "linux_nfc_api.h"

// class declaration
class NfcHandler {
public:
    NfcHandler();

private:
    void on_tag_arrival(nfc_tag_info_t*);
    void on_tag_departure();

    nfcTagCallback_t callbacks;
};

// global instance variable (used for dispatching to the correct instance)
static NfcHandler* global_handler_instance{nullptr};

// definitions
NfcHandler::NfcHandler() {
    // we could have also used a singleton instance,
    if (global_handler_instance) {
        throw std::runtime_error("Only one global nfc handler instance allowed");
    }

    this->callbacks.onTagArrival = [](nfc_tag_info_t* tag_info) { global_handler_instance->on_tag_arrival(tag_info); };

    this->callbacks.onTagDeparture = []() { global_handler_instance->on_tag_departure(); };

    global_handler_instance = this;

    nfcManager_doInitialize();
    nfcManager_registerTagCallback(&this->callbacks);
    nfcManager_enableDiscovery(DEFAULT_NFA_TECH_MASK, 0x01, 0, 0);
}

void NfcHandler::on_tag_arrival(nfc_tag_info_t*) {
    // handle tag arrival
}

void NfcHandler::on_tag_departure() {
    // handle tag departure
}

int main(int argc, char* argv[]) {

    NfcHandler handler;

    return 0;
}