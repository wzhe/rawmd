/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/noncopyable.h>
#include <base/Mutex.h>
#include <base/StringPiece.h>
#include <base/Types.h>
#include <net/Callbacks.h>
//#include <net/Buffer.h>
#include <net/InetAddress.h>

//#include <boost/any.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/scoped_ptr.hpp>
//#include <boost/shared_ptr.hpp>
#include <memory>   //shared_ptr unique_ptr enable_shared_from_this

namespace rawmd
{
namespace net
{

class Channel;
class EventLoop;
class Socket;

///
/// TCP connection, for both client and server usage.
///
/// This is an interface class, so don't expose too much details.
class TcpConnection : rawmd::noncopyable,
                      public std::enable_shared_from_this<TcpConnection>
{
    public:
        /// Constructs a TcpConnection with a connected sockfd
        ///
        /// User should not create this object.
        TcpConnection(EventLoop* loop,
                const string& name,
                int sockfd,
                const InetAddress& localAddr,
                const InetAddress& peerAddr);
        ~TcpConnection();

        EventLoop* getLoop() const { return loop_; }
        const string& name() const { return name_; }
        const InetAddress& localAddress() { return localAddr_; }
        const InetAddress& peerAddress() { return peerAddr_; }
        bool connected() const { return state_ == kConnected; }

        void setConnectionCallback(const ConnectionCallback& cb)
        { connectionCallback_ = cb; }

        void setMessageCallback(const MessageCallback& cb)
        { messageCallback_ = cb; }

        /// Internal use only.
        void setCloseCallback(const CloseCallback& cb)
        { closeCallback_ = cb; }

        // called when TcpServer accepts a new connection
        void connectEstablished();   // should be called only once
        // called when TcpServer has removed me from its map
        void connectDestroyed();  // should be called only once

    private:
        enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
        void handleRead(Timestamp receiveTime);
        void handleClose();
        void handleError();
        void setState(StateE s) { state_ = s; }

        EventLoop* loop_;			// 所属EventLoop
        string name_;				// 连接名
        StateE state_;  // FIXME: use atomic variable
        // we don't expose those classes to client.
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        InetAddress localAddr_;
        InetAddress peerAddr_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        CloseCallback closeCallback_;
};

}
}

