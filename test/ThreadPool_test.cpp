#include <base/ThreadPool.h>
#include <base/CountDownLatch.h>
#include <base/CurrentThread.h>

#include <functional>
#include <stdio.h>

void print()
{
  printf("tid=%d\n", rawmd::CurrentThread::tid());
}

void printString(const std::string& str)
{
  printf("tid=%d, str=%s\n", rawmd::CurrentThread::tid(), str.c_str());
}

int main()
{
  rawmd::ThreadPool pool("MainThreadPool");
  pool.start(5);

  pool.run(print);
  pool.run(print);
  for (int i = 0; i < 100; ++i)
  {
    char buf[32];
    snprintf(buf, sizeof buf, "task %d", i);
    pool.run(std::bind(printString, std::string(buf)));
  }

  rawmd::CountDownLatch latch(1);
  pool.run(std::bind(&rawmd::CountDownLatch::countDown, &latch));
  latch.wait();
  pool.stop();
}
