/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/noncopyable.h>
#include <base/CurrentThread.h>
#include <base/Thread.h>
#include <base/Timestamp.h>

#include <vector>
#include <memory>

namespace rawmd
{
namespace net
{

class Channel;
class Poller;

///
/// Reactor, at most one per thread.
///
/// This is an interface class, so don't expose too much details
class EventLoop : rawmd::noncopyable
{
public:
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
    Timestamp pollReutrnTime() const { return pollReturnTime_; }

    // internal usage
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

    static EventLoop* getEventLoopOfCurrentThread();
private:
    void abortNotInLoopThread();
    void printActiveChannels() const; // DEBUG

    typedef std::vector<Channel*> ChannelList;

    bool looping_;  // atomic 
    bool quit_;     // autmic
    bool eventHandling_;    // atomic
    const pid_t threadId_;  // 当前对象所属线程ID
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;    // Poller返回的活动通道
    Channel* currentActiveChannel_; // 当前正在处理的活动通道

};

}
}