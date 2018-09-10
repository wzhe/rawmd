//编写一个程序，开启3个线程，这3个线程的ID分别为A、B、C，
//每个线程将自己的ID在屏幕上打印10遍，要求输出结果必须按ABC的顺序显示；
//如：ABCABC….依次递推。

#include <base/CountDownLatch.h>
#include <base/Thread.h>

#include <stdio.h>
#include <functional>

rawmd::CountDownLatch latch(3*10);
rawmd::MutexLock mutex;


void ThreadProc(char name, int order)
{
    
    for(int i = 0; i < 10; ++i)
    {
        while(order != latch.getCount()%3)
        {
            ;
        }
        rawmd::MutexLockGuard lock(mutex);
        printf("%c",name);
        latch.countDown();
    }
}

int main()
{
    rawmd::Thread t1(std::bind(ThreadProc, 'A',0));
    rawmd::Thread t2(std::bind(ThreadProc, 'B',2));
    rawmd::Thread t3(std::bind(ThreadProc, 'C',1));
    t1.start();
    t2.start();
    t3.start();

    t1.join();
    t2.join();
    t3.join();
    printf("\n");
}