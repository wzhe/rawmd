set(base_SRCS
    Condition.cpp
    CountDownLatch.cpp
    Date.cpp
    Exception.cpp
    Logging.cpp
    LogStream.cpp
    Thread.cpp
    ThreadPool.cpp
    Timestamp.cpp
    TimeZone.cpp
  )
add_library(rawmd_base ${base_SRCS})
target_link_libraries(rawmd_base pthread)
#target_link_libraries(rawmd_base pthread rt)

install(TARGETS rawmd_base DESTINATION lib)
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/rawmd/base)