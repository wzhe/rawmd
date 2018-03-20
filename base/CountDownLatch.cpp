/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#include <base/CountDownLatch.h>

using namespace rawmd;

CountDownLatch::CountDownLatch(int count)
    : mutex_(),
      condition_(mutex_),
      count_(count)
{
}

void CountDownLatch::wait()
{
    MutexLockGuard lock(mutex_);
    while (count_ > 0)
    {
        condition_.wait();
    }
}

void CountDownLatch::coundDown()
{
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0)
    {
        condition_.notifyAll();
    }
}

int CountDownLatch::getCount() const
{
    MutexLockGuard lock(mutex_);
    return count_;
}