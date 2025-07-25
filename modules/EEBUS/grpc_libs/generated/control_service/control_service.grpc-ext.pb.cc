#include "control_service/control_service.grpc-ext.pb.h"

#include <mutex>
#include <condition_variable>

namespace control_service {

bool CallStartService(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::EmptyRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->StartService(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallStopService(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::EmptyRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->StopService(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallSetConfig(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::SetConfigRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->SetConfig(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallStartSetup(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::EmptyRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->StartSetup(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallAddEntity(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::AddEntityRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->AddEntity(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallRemoveEntity(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::RemoveEntityRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->RemoveEntity(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallRegisterRemoteSki(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::RegisterRemoteSkiRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->RegisterRemoteSki(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallAddUseCase(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::AddUseCaseRequest& request,
    control_service::AddUseCaseResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->AddUseCase(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

} // namespace control_service

