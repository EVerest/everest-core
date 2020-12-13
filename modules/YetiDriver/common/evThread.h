#include <thread>
#include <future>
#include <chrono>

class evThread {
    public:
        evThread();
        ~evThread();

        bool shouldExit();
        std::thread handle;

    private:
        std::promise<void> exitSignal;
        std::future<void> exitFuture;
};
