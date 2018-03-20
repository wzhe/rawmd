#include <base/Mutex.h>
#include <base/Thread.h>
#include <base/Timestamp.h>

#include <functional>
#include <memory>
#include <vector>
#include <stdio.h>

using namespace rawmd;
using namespace std;

MutexLock g_mutex;
vector<int> g_vec;
const int kCount = 10*1000*1000;


void threadFunc()
{
    for (int i = 0; i < kCount; ++i)
    {
        MutexLockGuard lock(g_mutex);
        g_vec.push_back(i);
    }
}

int main()
{
    const int kMaxThreads = 8;
    g_vec.reserve(kMaxThreads * kCount);

    Timestamp start(Timestamp::now());
    for (int i=0; i < kCount; ++i)
    {
        g_vec.push_back(i);
    }

    printf("single thread without lock %f\n", timeDifference(Timestamp::now(), start));

    start = Timestamp::now();
    threadFunc();
    printf("single thread with lock %f\n", timeDifference(Timestamp::now(), start));

    for (int nthreads = 1; nthreads < kMaxThreads; ++nthreads)
    {
        vector<unique_ptr<Thread>> threads;
        g_vec.clear();
        start = Timestamp::now();
        for (int i = 0; i < nthreads; ++i)
        {
            threads.push_back(static_cast<unique_ptr<Thread>>(new Thread(&threadFunc)));
            threads.back()->start();
        }
        for (int i = 0; i < nthreads; ++i)
        {
            threads[i]->join();
        }
        printf("%d thread(s) with lock %f\n", nthreads, timeDifference(Timestamp::now(),start));
    }
}