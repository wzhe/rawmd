set(net_SRCS
    Channel.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    Poller.cpp
    poller/DefaultPoller.cpp
    poller/EPollPoller.cpp
    poller/PollPoller.cpp
    Timer.cpp
    TimerQueue.cpp
)
add_library(rawmd_net ${net_SRCS})
target_link_libraries(rawmd_net rawmd_base)

add_subdirectory(preface)
