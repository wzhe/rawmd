/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/noncopyable.h>
#include <base/AtomicInteger.h>
#include <base/Types.h>

#include <pthread.h>
#include <functional>

namespace rawmd
{

class Thread : rawmd::noncopyable
{
public:
    typedef std::function<void ()> ThreadFunc;

    explicit Thread(const ThreadFunc&, const string& name = string());
    ~Thread();

    void start();
    int join(); //return pthread_join()


    bool started() const { return started_; }
  // pthread_t pthreadId() const { return pthreadId_; }
  pid_t tid() const { return tid_; }
  const string& name() const { return name_; }

    static int numCreated() { return numCreated_.get(); }

private:
    static void* startThread(void* thread);
    void runInThread(); //是普通成员函数，隐含的第一个参数是 Thread*(this)
                      //调用是时候是 thiscall约定，某一个寄存器会保护this指针
                      //不能直接作为pthread_create的入口函数。

    bool        started_;
    pthread_t   pthreadId_;
    pid_t       tid_;
    ThreadFunc  func_;
    string      name_;

    static AtomicInt32 numCreated_;
};

}