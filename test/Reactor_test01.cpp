#include <net/EventLoop.h>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


using namespace rawmd;
using namespace rawmd::net;

void threadFunc()
{
	printf("threadFunc(): pid = %d, tid = %d\n",
		getpid(), CurrentThread::tid());

	EventLoop loop;
	loop.loop();
}

int main(void)
{
	printf("main(): pid = %d, tid = %d\n",
		getpid(), CurrentThread::tid());

	EventLoop loop;

	Thread t(threadFunc);
	t.start();

	loop.loop();
	t.join();
	return 0;
}
