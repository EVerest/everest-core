// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <DiscoveryEventReader.hpp>

#include <everest/logging.hpp>

DiscoveryEventReader::DiscoveryEventReader(std::shared_ptr<control_service::ControlService::Stub> stub,
                                           EventCallback event_callback, DisconnectionCallback disconnection_callback) :
    stub(std::move(stub)),
    event_callback(std::move(event_callback)),
    disconnection_callback(std::move(disconnection_callback)),
    done_future_(done_promise_.get_future()) {
}

DiscoveryEventReader::~DiscoveryEventReader() {
    if (started_) {
        stop();
        done_future_.wait();
    }
}

void DiscoveryEventReader::start() {
    started_ = true;
    if (!this->stub) {
        return;
    }
    this->stub->async()->SubscribeDiscoveryEvents(&this->context, &this->request, this);
    this->StartRead(&this->response);
    this->StartCall();
}

void DiscoveryEventReader::stop() {
    bool expected = false;
    if (this->stopped_.compare_exchange_strong(expected, true)) {
        this->context.TryCancel();
    }
}

void DiscoveryEventReader::OnReadDone(bool ok) {
    if (ok) {
        if (event_callback) {
            event_callback(this->response);
        }
        StartRead(&this->response);
    }
}

void DiscoveryEventReader::OnDone(const grpc::Status& s) {
    done_promise_.set_value();
    if (s.error_code() == grpc::StatusCode::CANCELLED) {
        return;
    }
    if (!s.ok()) {
        EVLOG_error << "DiscoveryEventReader stream failed: " << s.error_message();
    } else {
        EVLOG_warning << "DiscoveryEventReader stream closed by server unexpectedly.";
    }
    if (disconnection_callback) {
        disconnection_callback();
    }
}
