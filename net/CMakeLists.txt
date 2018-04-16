set(net_SRCS
    Channel.cpp
    EventLoop.cpp
    Poller.cpp
    poller/DefaultPoller.cpp
    poller/EPollPoller.cpp
    poller/PollPoller.cpp
)
add_library(rawmd_net ${net_SRCS})
target_link_libraries(rawmd_net rawmd_base)

add_subdirectory(preface)