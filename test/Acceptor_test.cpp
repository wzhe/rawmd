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
	printf("newConnection(): accepted a new connection from %s\n",
			peerAddr.toHostPort().c_str());
	::write(sockfd, "How are you?\n", 13);
	::close(sockfd);
}

int main()
{
  printf("main pid=%d, tid=%d\n", ::getpid(), CurrentThread::tid());

  net::InetAddress listenAddr(9981);
  net::EventLoop loop;

  net::Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();
  loop.loop();
}
