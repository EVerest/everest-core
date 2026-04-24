// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <UseCaseEventReader.hpp>

#include <everest/logging.hpp>

#include "common_types/types.pb.h"
#include "control_service/messages.pb.h"

UseCaseEventReader::UseCaseEventReader(
    std::shared_ptr<control_service::ControlService::Stub> stub,
    std::function<void(const control_service::SubscribeUseCaseEventsResponse&)> event_callback,
    std::function<void()> disconnection_callback) :
    stub(std::move(stub)),
    event_callback(std::move(event_callback)),
    disconnection_callback(std::move(disconnection_callback)),
    done_future_(done_promise_.get_future()) {
}

UseCaseEventReader::~UseCaseEventReader() {
    if (started_) {
        stop();
        done_future_.wait();
    }
}

void UseCaseEventReader::start(const common_types::EntityAddress& entity_address,
                               const control_service::UseCase& use_case) {
    started_ = true;
    if (!this->stub) {
        return;
    }
    this->request.mutable_entity_address()->CopyFrom(entity_address);
    this->request.mutable_use_case()->CopyFrom(use_case);
    this->stub->async()->SubscribeUseCaseEvents(&context, &request, this);
    this->StartRead(&this->response);
    this->StartCall();
}

void UseCaseEventReader::stop() {
    this->context.TryCancel();
}

void UseCaseEventReader::OnReadDone(bool ok) {
    if (ok) {
        if (event_callback) {
            event_callback(this->response);
        }
        StartRead(&this->response);
    }
}

void UseCaseEventReader::OnDone(const grpc::Status& s) {
    done_promise_.set_value();
    if (s.error_code() == grpc::StatusCode::CANCELLED) {
        return; // Normal shutdown via stop()
    }
    if (!s.ok()) {
        EVLOG_error << "UseCaseEventReader stream failed: " << s.error_message();
    } else {
        EVLOG_warning << "UseCaseEventReader stream closed by server unexpectedly.";
    }
    if (disconnection_callback) {
        disconnection_callback();
    }
}
