/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#include <net/TcpConnection.h>

#include <base/Logging.h>
#include <net/Channel.h>
#include <net/EventLoop.h>
#include <net/Socket.h>
#include <net/SocketsOps.h>

#include <functional>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

using namespace rawmd;
using namespace rawmd::net;
using std::placeholders::_1;
/*
void rawmd::net::defaultConnectionCallback(const TcpConnectionPtr& conn)
{
  LOG_TRACE << conn->localAddress().toIpPort() << " -> "
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
}

void rawmd::net::defaultMessageCallback(const TcpConnectionPtr&,
                                        Buffer* buf,
                                        Timestamp)
{
  buf->retrieveAll();
}
*/
TcpConnection::TcpConnection(EventLoop* loop,
                             const string& nameArg,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
  : loop_(CHECK_NOTNULL(loop)),
    name_(nameArg),
    state_(kConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr)/*,
    highWaterMark_(64*1024*1024)*/
{
    // 通道可读事件到来的时候，回调TcpConnection::handleRead，_1是事件发生时间
    channel_->setReadCallback(
            std::bind(&TcpConnection::handleRead, this, _1));
    // 连接关闭，回调TcpConnection::handleClose
    channel_->setCloseCallback(
            std::bind(&TcpConnection::handleClose, this));
    // 发生错误，回调TcpConnection::handleError
    channel_->setErrorCallback(
            std::bind(&TcpConnection::handleError, this));
    LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this
        << " fd=" << sockfd;
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
        << " fd=" << channel_->fd();
}

void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    LOG_TRACE << "[3] usecount=" << shared_from_this().use_count();
    channel_->tie(shared_from_this());
    channel_->enableReading();  // TcpConnection所对应的通道加入到Poller关注

    connectionCallback_(shared_from_this());
    LOG_TRACE << "[4] usecount=" << shared_from_this().use_count();
}

void TcpConnection::connectDestroyed()
{
    loop_->assertInLoopThread();
    if (state_ == kConnected)
    {
        setState(kDisconnected);
        channel_->disableAll();

        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    /*
       loop_->assertInLoopThread();
       int savedErrno = 0;
       ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
       if (n > 0)
       {
       messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
       }
       else if (n == 0)
       {
       handleClose();
       }
       else
       {
       errno = savedErrno;
       LOG_SYSERR << "TcpConnection::handleRead";
       handleError();
       }
       */
    loop_->assertInLoopThread();
    int savedErrno = 0;
    char buf[65536];
    ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
    if (n > 0)
    {
        messageCallback_(shared_from_this(), buf, n);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        LOG_SYSERR << "TcpConnection::handleRead";
        handleError();
    }

}

void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    LOG_TRACE << "fd = " << channel_->fd() << " state = " << state_;
    assert(state_ == kConnected || state_ == kDisconnecting);
    // we don't close fd, leave it to dtor, so we can find leaks easily.
    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);     // 这一行，可以不调用
    LOG_TRACE << "[7] usecount=" << guardThis.use_count();
    // must be the last line
    closeCallback_(guardThis);  // 调用TcpServer::removeConnection
    LOG_TRACE << "[11] usecount=" << guardThis.use_count();
}

void TcpConnection::handleError()
{
    int err = sockets::getSocketError(channel_->fd());
    LOG_ERROR << "TcpConnection::handleError [" << name_
        << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}
