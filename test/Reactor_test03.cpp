
#include <base/Logging.h>
#include <net/Channel.h>
#include <net/EventLoop.h>

#include <functional>

#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>
#include <unistd.h>

using namespace rawmd;
using namespace rawmd::net;
using std::placeholders::_1;

EventLoop* g_loop;
int timerfd;

void timeout(Timestamp receiveTime)
{
	printf("Timeout!\n");
	uint64_t howmany;
	::read(timerfd, &howmany, sizeof howmany);
	//g_loop->quit();
}

int main(void)
{
	Logger::setLogLevel(Logger::TRACE);
	EventLoop loop;
	g_loop = &loop;

	timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	Channel channel(&loop, timerfd);
	channel.setReadCallback(std::bind(timeout, _1));
	channel.enableReading();

	struct itimerspec howlong;
	memset(&howlong, 0x00, sizeof howlong);
	howlong.it_value.tv_sec = 1;
	::timerfd_settime(timerfd, 0, &howlong, NULL);

	loop.loop();

	::close(timerfd);
}



