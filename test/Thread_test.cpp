#include <base/Thread.h>
#include <base/CurrentThread.h>

#include <string>
#include <stdio.h>
#include <functional>
#include <unistd.h>

void mysleep(int seconds)
{
    timespec t = { seconds, 0 };
    nanosleep(&t, NULL);
}

void threadFunc()
{
    printf("tid=%d\n", rawmd::CurrentThread::tid());
}

void threadFunc2(int x)
{
    printf("tid=%d, x=%d\n", rawmd::CurrentThread::tid(), x);
}

void threadFunc3()
{
    printf("tid=%d\n", rawmd::CurrentThread::tid());
    mysleep(1);
}

class Foo
{
public:
    explicit Foo(double x)
        : x_(x)
    {
    }

    void memberFunc()
    {
        printf("tid = %d, Foo::x=%f\n", rawmd::CurrentThread::tid(), x_);
    }

    void memberFunc2(const std::string& text)
    {
        printf("tid=%d, Foo::x=%f, text=%s \n", rawmd::CurrentThread::tid(), x_, text.c_str());
    }
private:
    double x_;
};

class Boo
{
public:
    explicit Boo(std::string threadName)
        : loop_(true),
          thread_(std::bind(&Boo::threadProc, this), threadName)
    {}

    void start()
    {
        thread_.start();
    }

    void stop()
    {
        loop_ = false;
    }

    void threadProc()
    {
        while (loop_) {
             printf("thread_.tid=%d\n", thread_.tid());
            timespec t = { 5, 0 };
            nanosleep(&t, NULL);
        }
    }
private:
    bool   loop_;
    rawmd::Thread thread_;
};

int main()
{
    printf("pid=%d, tid=%d\n", ::getpid(), rawmd::CurrentThread::tid());

    rawmd::Thread t1(threadFunc);
    t1.start();
    printf("t1.tid=%d\n", t1.tid());
    t1.join();
    printf("t1.tid=%d\n", t1.tid()); //代码中目前的问题，回头改为和muduo最新版（2018年3月19日）一样的实现
    rawmd::Thread t2(std::bind(threadFunc2, 55), 
                        "thread of free function with argument");
    t2.start();
    printf("t2.tid=%d\n", t2.tid());
    t2.join();
    printf("t2.tid=%d\n", t2.tid());
    Foo foo(88.23);
    rawmd::Thread t3(std::bind(&Foo::memberFunc, &foo),
                        "thread for member funciton without argument");
    t3.start();
    t3.join();

    rawmd::Thread t4(std::bind(&Foo::memberFunc2, &foo, std::string("Read And Write Muduo")));
    t4.start();
    t4.join();

    {
        rawmd::Thread t5(threadFunc3);
        t5.start();
        // t5 may destruct eariiler than thread creation.
    }

    mysleep(2);
    {
        rawmd::Thread t6(threadFunc3);
        t6.start();
        mysleep(2);
        // t6 destruct later than thread creation.
    }

    sleep(2);
    Boo b("hwqe");
    b.start();

    printf("number of created threads %d\n", rawmd::Thread::numCreated());


}