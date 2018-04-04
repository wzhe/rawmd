#include <base/CountDownLatch.h>
#include <base/Thread.h>

#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>


using namespace rawmd;
using namespace std;
using std::placeholders::_1;

class CountDownLatch_test2
{
public:
    explicit CountDownLatch_test1(int numThreads)
        : latch_(1)
      {
        for (int i=0; i < numThreads; ++i)
        {
            char name[32];
            snprintf(name, sizeof name, "work thread %d", i);
            threads_.push_back( static_cast<unique_ptr<Thread>> (new Thread(
                    bind(&CountDownLatch_test1::threadFunc, this), string(name))));
            threads_.back()->start();
        }
      }

      void wait()
      {
          latch_.wait();
      }

      void joinAll()
      {
        for_each(threads_.begin(), threads_.end(), bind(&Thread::join, _1));
      }

private:
    void threadFunc()
    {
        sleep(3);
        latch_.countDown();
        printf("tid=%d, %s started\n",
                CurrentThread::tid(),
                CurrentThread::name());
        
        printf("tid=%d, %s stopped\n",
                CurrentThread::tid(),
                CurrentThread::name());
    }
    CountDownLatch latch_;
    vector<unique_ptr<Thread>> threads_;
};

int main()
{
    printf("pid=%d, tiud=%d\n", ::getpid(), CurrentThread::tid());
    CountDownLatch_test2 t(3);
    t.wait();
    printf("pid=%d, tid=%d %s running ...\n", ::getpid(), CurrentThread::tid(), CurrentThread::name());
    t.joinAll();
    
    printf("number of created threads %d\n", Thread::numCreated());
}
