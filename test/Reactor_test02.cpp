#include <net/EventLoop.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

using namespace rawmd;
using namespace rawmd::net;

EventLoop* g_loop;

void threadFunc()
{
	g_loop->loop();
}

int main(void)
{
	EventLoop loop;
	g_loop = &loop;
	Thread t(threadFunc);
	t.start();
	t.join();
	return 0;
}

