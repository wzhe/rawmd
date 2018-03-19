/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/
#include <base/Thread.h>
#include <base/CurrentThread.h>
#include <base/Exception.h>

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
namespace rawmd
{
namespace CurrentThread
{
      // __thread修饰的变量是线程局部存储的
    __thread int t_cachedTid = 0; //线程真实PID（tid)的缓存，
                                  //是为了减少系统调用的次数，提高获取效率
    __thread char t_tidString[32]; // 这是tid的字符串表示形式
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "unknown";

    static_assert(sizeof(pid_t) == sizeof(int),
              "pid_t should be same size as int");
}

namespace detail
{
pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork()
{
    rawmd::CurrentThread::t_cachedTid = 0;
    rawmd::CurrentThread::t_threadName = "main";
    CurrentThread::tid();
    // no need to call pthread_atfork(NULL, NULL, &afterFork);
}

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        rawmd::CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer init;
}
}

using namespace rawmd;

void CurrentThread::cacheTid()
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        int n= snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
        assert(n >= 6);
        (void)n;
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const string& n)
  : started_(false),
    pthreadId_(0),
    tid_(0),
    func_(func),
    name_(n)
{
    numCreated_.increment();
}

Thread::~Thread()
{
    // no join
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    errno = pthread_create(&pthreadId_, NULL, &startThread, this);
    if (errno != 0 )
    {
        // LOG_SYSFATAL << "Failed in pthread_create";
    }
}

int Thread::join()
{
    assert(started_);
    return pthread_join(pthreadId_, NULL);
}

void* Thread::startThread(void* obj)
{
    Thread* thread = static_cast<Thread*>(obj);
    thread->runInThread();
    return NULL;
}

void Thread::runInThread()
{
    tid_ = CurrentThread::tid();
    rawmd::CurrentThread::t_threadName = name_.c_str();
    try
    {
        func_();
        rawmd::CurrentThread::t_threadName = "finished";
    }
  catch (const Exception& ex)
  {
    rawmd::CurrentThread::t_threadName = "crashed";
    fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
    abort();
  }
  catch (const std::exception& ex)
  {
    rawmd::CurrentThread::t_threadName = "crashed";
    fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    abort();
  }
  catch (...)
  {
    rawmd::CurrentThread::t_threadName = "crashed";
    fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
    throw; // rethrow
  }
}

