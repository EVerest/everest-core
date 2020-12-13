#ifndef UTILS_THREAD_HPP
#define UTILS_THREAD_HPP

#include <chrono>
#include <future>
#include <thread>

namespace Everest {
class Thread {
public:
    Thread();
    ~Thread();

    bool shouldExit();
    void operator=(std::thread&&);

private:
    std::thread handle;
    std::promise<void> exitSignal;
    std::future<void> exitFuture;
};
} // namespace Everest

#endif // UTILS_THREAD_HPP
