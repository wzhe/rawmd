/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once


#include <base/noncopyable.h>
#include <base/Types.h>
#include <net/TcpConnection.h>

#include <map>
//#include <boost/scoped_ptr.hpp>
#include <memory>

namespace rawmd
{
namespace net
{

class Acceptor;
class EventLoop;

///
/// TCP server, supports single-threaded and thread-pool models.
///
/// This is an interface class, so don't expose too much details.
class TcpServer : rawmd::noncopyable
{
    public:
        //typedef boost::function<void(EventLoop*)> ThreadInitCallback;

        //TcpServer(EventLoop* loop, const InetAddress& listenAddr);
        TcpServer(EventLoop* loop,
                const InetAddress& listenAddr,
                const string& nameArg);
        ~TcpServer();  // force out-line dtor, for scoped_ptr members.

        const string& hostport() const { return hostport_; }
        const string& name() const { return name_; }

        /// Starts the server if it's not listenning.
        ///
        /// It's harmless to call it multiple times.
        /// Thread safe.
        void start();

        /// Set connection callback.
        /// Not thread safe.
        // 设置连接到来或者连接关闭回调函数
        void setConnectionCallback(const ConnectionCallback& cb)
        { connectionCallback_ = cb; }

        /// Set message callback.
        /// Not thread safe.
        // 设置消息到来回调函数
        void setMessageCallback(const MessageCallback& cb)
        { messageCallback_ = cb; }


    private:
        /// Not thread safe, but in loop
        void newConnection(int sockfd, const InetAddress& peerAddr);
        /// Thread safe.
        void removeConnection(const TcpConnectionPtr& conn);

        typedef std::map<string, TcpConnectionPtr> ConnectionMap;

        EventLoop* loop_;  // the acceptor loop
        const string hostport_;		// 服务端口
        const string name_;			// 服务名
        std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        bool started_;
        // always in loop thread
        int nextConnId_;				// 下一个连接ID
        ConnectionMap connections_;	// 连接列表
};

}
}

