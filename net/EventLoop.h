/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/noncopyable.h>
#include <base/Mutex.h>
#include <base/Thread.h>
#include <base/Timestamp.h>
#include <net/Callbacks.h>
#include <net/TimerId.h>

#include <vector>
#include <memory>

namespace rawmd
{
namespace net
{

class Channel;
class Poller;
class TimerQueue;
///
/// Reactor, at most one per thread.
///
/// This is an interface class, so don't expose too much details
class EventLoop : rawmd::noncopyable
{
    public:
        typedef std::function<void()> Functor;
        EventLoop();
        ~EventLoop();   //force out-line dtor, for scoped_ptr members.
        //(I use unique_ptr whicdoes not have unique_ptr::release method
        //，its inner class object will live as long as the program.)

        /// Loops forever.
        ///
        /// Must be called in the same thread as creation of the object.
        ///
        void loop();

        void quit();

        ///
        /// Time when poll returns, usually means data arrivial.
        ///
        Timestamp pollReturnTime() const { return pollReturnTime_; }

        /// Runs callback immediately in the loop thread.
        /// It wakes up the loop, and run the cb.
        /// If in the same loop thread, cb is run within the function.
        /// Safe to call from other threads.
        void runInLoop(const Functor& cb);
        /// Queues callback in the loop thread.
        /// Runs after finish pooling.
        /// Safe to call from other threads.
        void queueInLoop(const Functor& cb);

        // timers

        ///
        /// Runs callback at 'time'.
        /// Safe to call from other threads.
        ///
        TimerId runAt(const Timestamp& time, const TimerCallback& cb);
        ///
        /// Runs callback after @c delay seconds.
        /// Safe to call from other threads.
        ///
        TimerId runAfter(double delay, const TimerCallback& cb);
        ///
        /// Runs callback every @c interval seconds.
        /// Safe to call from other threads.
        ///
        TimerId runEvery(double interval, const TimerCallback& cb);
        ///
        /// Cancels the timer.
        /// Safe to call from other threads.
        ///
        void cancel(TimerId timerId);
        // internal usage
        void wakeup();
        void updateChannel(Channel* channel);   //在Poller中添加或者更新通道
        void removeChannel(Channel* channel);   //在Poller中移除通道

        void assertInLoopThread()
        {
            if (!isInLoopThread())
            {
                abortNotInLoopThread();
            }
        }
        bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

        bool eventHandling() const { return eventHandling_; }
        static EventLoop* getEventLoopOfCurrentThread();
    private:
        void abortNotInLoopThread();
        void handleRead();  // waked up
        void doPendingFunctors();
        void printActiveChannels() const; // DEBUG

        typedef std::vector<Channel*> ChannelList;

        bool looping_;  // atomic 
        bool quit_;     // autmic
        bool eventHandling_;    // atomic
        bool callingPendingFunctors_; /* atomic */
        const pid_t threadId_;  // 当前对象所属线程ID
        Timestamp pollReturnTime_;
        std::unique_ptr<Poller> poller_;
        std::unique_ptr<TimerQueue> timerQueue_;
        int wakeupFd_;              // 用于eventfd
        std::unique_ptr<Channel> wakeupChannel_;    // 该通道将会纳入poller_来管理
        ChannelList activeChannels_;    // Poller返回的活动通道
        Channel* currentActiveChannel_; // 当前正在处理的活动通道
        MutexLock mutex_;
        std::vector<Functor> pendingFunctors_; // @BuardedBy mutex_
};

}
}
