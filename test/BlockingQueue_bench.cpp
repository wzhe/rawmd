#include <base/BlockingQueue.h>
#include <base/CountDownLatch.h>
#include <base/Thread.h>
#include <base/Timestamp.h>

#include <functional>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <stdio.h>
#include <algorithm>
#include <unistd.h>

using namespace std;
using namespace rawmd;
using std::placeholders::_1;

class Bench
{
 public:
  Bench(int numThreads)
    : latch_(numThreads)
  {
    for (int i = 0; i < numThreads; ++i)
    {
      char name[32];
      snprintf(name, sizeof name, "work thread %d", i);
      threads_.push_back(static_cast<unique_ptr<Thread>>(new Thread(
            bind(&Bench::threadFunc, this), string(name))));
    }
    for_each(threads_.begin(), threads_.end(), bind(&Thread::start, _1));
  }

  void run(int times)
  {
    printf("waiting for count down latch\n");
    latch_.wait();
    printf("all threads started\n");
    for (int i = 0; i < times; ++i)
    {
      Timestamp now(Timestamp::now());
      queue_.put(now);
      usleep(1000);
    }
  }

  void joinAll()
  {
    for (size_t i = 0; i < threads_.size(); ++i)
    {
      queue_.put(Timestamp::invalid());
    }

    for_each(threads_.begin(), threads_.end(), bind(&Thread::join, _1));
  }

 private:

  void threadFunc()
  {
    printf("tid=%d, %s started\n",
           CurrentThread::tid(),
           CurrentThread::name());

    std::map<int, int> delays;
    latch_.countDown();
    bool running = true;
    while (running)
    {
      Timestamp t(queue_.take());
      Timestamp now(Timestamp::now());
      if (t.valid())
      {
        int delay = static_cast<int>(timeDifference(now, t) * 1000000);
        // printf("tid=%d, latency = %d us\n",
        //        CurrentThread::tid(), delay);
        ++delays[delay];
      }
      running = t.valid();
    }

    printf("tid=%d, %s stopped\n",
           CurrentThread::tid(),
           CurrentThread::name());
    for (std::map<int, int>::iterator it = delays.begin();
        it != delays.end(); ++it)
    {
      printf("tid = %d, delay = %d, count = %d\n",
             CurrentThread::tid(),
             it->first, it->second);
    }
  }

  BlockingQueue<Timestamp> queue_;
  CountDownLatch latch_;
  vector<unique_ptr<Thread>> threads_;
};

int main(int argc, char* argv[])
{
  int threads = argc > 1 ? atoi(argv[1]) : 1;

  Bench t(threads);
  t.run(10000);
  t.joinAll();
}
