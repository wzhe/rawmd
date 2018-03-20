add_executable(atomicinteger_test AtomicInteger_test.cpp)

add_executable(exception_test Exception_test.cpp)
target_link_libraries(exception_test rawmd_base)

add_executable(mutex_test Mutex_test.cpp)
target_link_libraries(mutex_test rawmd_base)

add_executable(thread_test Thread_test.cpp)
target_link_libraries(thread_test rawmd_base)

add_executable(timestamp_test Timestamp_test.cpp)
target_link_libraries(timestamp_test rawmd_base)