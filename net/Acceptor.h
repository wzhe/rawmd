/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <functional>

#include <base/noncopyable.h>
#include <net/Channel.h>
#include <net/Socket.h>

namespace rawmd
{
namespace net
{

class EventLoop;
class InetAddress;

///
/// Acceptor of incoming TCP connections.
///
class Acceptor : rawmd::noncopyable
{
    public:
        typedef std::function<void (int sockfd,
                const InetAddress&)> NewConnectionCallback;

        Acceptor(EventLoop* loop, const InetAddress& listenAddr);
        ~Acceptor();

        void setNewConnectionCallback(const NewConnectionCallback& cb)
        { newConnectionCallback_ = cb; }

        bool listenning() const { return listenning_; }
        void listen();

    private:
        void handleRead();

        EventLoop* loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        bool listenning_;
        int idleFd_;
};

}
}

