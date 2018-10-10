//一个全局变量tally，两个线程并发执行（代码段都是ThreadProc)，
//问两个线程都结束后，tally取值范围。


#include <base/CountDownLatch.h>
#include <base/Thread.h>

#include <stdio.h>

rawmd::CountDownLatch latch(1);
int tally = 0;//glable

void ThreadProc()
{
    latch.wait();
    for(int i = 1; i <= 1000; i++)
        tally += 1;
}

int main()
{
    rawmd::Thread t1(ThreadProc);
    rawmd::Thread t2(ThreadProc);
    t1.start();
    t2.start();

    latch.countDown();
    t1.join();
    t2.join();
    printf("tally %d\n",tally);
}