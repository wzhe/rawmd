/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once


#include <base/noncopyable.h>
#include <base/Condition.h>
#include <base/Mutex.h>
#include <base/Thread.h>


namespace rawmd
{
namespace net
{

class EventLoop;

class EventLoopThread : rawmd::noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
    ~EventLoopThread();
    EventLoop* startLoop();	// 启动线程，该线程就成为了IO线程

private:
    void threadFunc();		// 线程函数

    EventLoop* loop_;			// loop_指针指向一个EventLoop对象
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    ThreadInitCallback callback_;		// 回调函数在EventLoop::loop事件循环之前被调用
};

}
}

