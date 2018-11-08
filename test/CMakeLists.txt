############ test for base library ##########################################

add_executable(atomicinteger_test AtomicInteger_test.cpp)

add_executable(blockingqueue_test BlockingQueue_test.cpp)
target_link_libraries(blockingqueue_test rawmd_base)

add_executable(blockingqueue_bench BlockingQueue_bench.cpp)
target_link_libraries(blockingqueue_bench rawmd_base)

add_executable(boundedblockingqueue_test BoundedBlockingQueue_test.cpp)
target_link_libraries(boundedblockingqueue_test rawmd_base)

add_executable(countdownlatch_test1 CountDownLatch_test1.cpp)
target_link_libraries(countdownlatch_test1 rawmd_base)

add_executable(countdownlatch_test2 CountDownLatch_test1.cpp)
target_link_libraries(countdownlatch_test2 rawmd_base)

add_executable(exception_test Exception_test.cpp)
target_link_libraries(exception_test rawmd_base)

add_executable(logging_test Logging_test.cpp)
target_link_libraries(logging_test rawmd_base)

add_executable(logstream_bench LogStream_bench.cpp)
target_link_libraries(logstream_bench rawmd_base)

add_executable(logstream_test LogStream_test.cpp)
target_link_libraries(logstream_test rawmd_base)

add_executable(minigtest_test Minigtest_test.cpp)

add_executable(mutex_test Mutex_test.cpp)
target_link_libraries(mutex_test rawmd_base)

add_executable(thread_test Thread_test.cpp)
target_link_libraries(thread_test rawmd_base)

add_executable(threadlocal_test ThreadLocal_test.cpp)
target_link_libraries(threadlocal_test rawmd_base)

add_executable(threadpool_test ThreadPool_test.cpp)
target_link_libraries(threadpool_test rawmd_base)

add_executable(timestamp_test Timestamp_test.cpp)
target_link_libraries(timestamp_test rawmd_base)

############ test for net library ##########################################

add_executable(acceptor_test Acceptor_test.cpp)
target_link_libraries(acceptor_test rawmd_net)

add_executable(acceptor_test02 Acceptor_test02.cpp)
target_link_libraries(acceptor_test02 rawmd_net)

add_executable(reactor_test01 Reactor_test01.cpp)
target_link_libraries(reactor_test01 rawmd_net)

add_executable(reactor_test02 Reactor_test02.cpp)
target_link_libraries(reactor_test02 rawmd_net)

add_executable(reactor_test03 Reactor_test03.cpp)
target_link_libraries(reactor_test03 rawmd_net)

############ test for fun ##########################################
add_executable(NoLockErrorTest NoLockErrorTest.cpp)
target_link_libraries(NoLockErrorTest rawmd_base)

add_executable(XunLeiProblem XunLeiProblem.cpp)
target_link_libraries(XunLeiProblem rawmd_base)
