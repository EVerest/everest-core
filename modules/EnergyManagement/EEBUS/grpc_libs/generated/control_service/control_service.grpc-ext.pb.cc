#include "control_service/control_service.grpc-ext.pb.h"

#include <condition_variable>
#include <mutex>

namespace control_service {

grpc::Status CallStartService(
    const std::shared_ptr<control_service::ControlService::Stub> &stub,
    control_service::EmptyRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->StartService(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallStopService(
    const std::shared_ptr<control_service::ControlService::Stub> &stub,
    control_service::EmptyRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->StopService(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallSetConfig(
    const std::shared_ptr<control_service::ControlService::Stub> &stub,
    control_service::SetConfigRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->SetConfig(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallStartSetup(
    const std::shared_ptr<control_service::ControlService::Stub> &stub,
    control_service::EmptyRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->StartSetup(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallAddEntity(
    const std::shared_ptr<control_service::ControlService::Stub> &stub,
    control_service::AddEntityRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->AddEntity(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallRemoveEntity(
    const std::shared_ptr<control_service::ControlService::Stub> &stub,
    control_service::RemoveEntityRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->RemoveEntity(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallRegisterRemoteSki(
    const std::shared_ptr<control_service::ControlService::Stub> &stub,
    control_service::RegisterRemoteSkiRequest& request,
    control_service::EmptyResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->RegisterRemoteSki(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallAddUseCase(
    const std::shared_ptr<control_service::ControlService::Stub> &stub,
    control_service::AddUseCaseRequest& request,
    control_service::AddUseCaseResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->AddUseCase(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

} // namespace control_service

