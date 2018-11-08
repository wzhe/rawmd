#include <net/Acceptor.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>

#include <functional>
#include <memory>
#include <string>
#include <stdio.h>
#include <algorithm>
#include <unistd.h>

using namespace rawmd;
using std::placeholders::_1;

void newConnection(int sockfd, const net::InetAddress& peerAddr)
{
	printf("newConnection(9981): accepted a new connection from %s\n",
			peerAddr.toHostPort().c_str());
	::write(sockfd, "How are you?\n", 13);
	::close(sockfd);
}

void newConnection02(int sockfd, const net::InetAddress& peerAddr)
{
	printf("newConnection(9982): accepted a new connection from %s\n",
			peerAddr.toHostPort().c_str());
	::write(sockfd, "I am fine\n", 11);
	::close(sockfd);
}
int main()
{
  printf("main pid=%d, tid=%d\n", ::getpid(), CurrentThread::tid());
  net::EventLoop loop;

  net::InetAddress listenAddr(9981);
  net::InetAddress listenAddr02(9982);

  net::Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();
  
  net::Acceptor acceptor02(&loop, listenAddr02);
  acceptor02.setNewConnectionCallback(newConnection02);
  acceptor02.listen();
  loop.loop();
}
