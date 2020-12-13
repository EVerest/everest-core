#include "evThread.h"

evThread::evThread() {
    exitFuture = exitSignal.get_future();
}

evThread::~evThread() {
    exitSignal.set_value();
    if (handle.joinable())
        handle.join();
}

bool evThread::shouldExit() {
    if (exitFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        return true;
    return false;
}
