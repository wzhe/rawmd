//有四个线程1、2、3、4。线程1的功能就是输出1，线程2的功能就是输出2，以此类推.........
//现在有四个文件ABCD。初始都为空。现要让四个文件呈如下格式：
//
// A：1 2 3 4 1 2....
// B：2 3 4 1 2 3....
// C：3 4 1 2 3 4....
// D：4 1 2 3 4 1....
// 请设计程序。

#include <base/CountDownLatch.h>
#include <base/Thread.h>

#include <stdio.h>
#include <functional>

rawmd::CountDownLatch latch(3*10);
rawmd::MutexLock mutex;

char buf_A[1024] = {};
char buf_B[1024] = {};
char buf_C[1024] = {};
char buf_D[1024] = {};

int cur_A = 0;
int cur_B = 0;
int cur_C = 0;
int cur_D = 0;

void ThreadProc(char name, int order)
{
    while(true)
    {
        while(order != latch.getCount()%3)
        {
            ;
        }
        rawmd::MutexLockGuard lock(mutex);
        printf("%c ",name);
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