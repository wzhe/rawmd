/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/noncopyable.h>
#include <base/Mutex.h>
#include <base/Condition.h>

#include <deque>
#include <assert.h>

namespace rawmd
{

template<typename T>
class BlockingQueue : rawmd::noncopyable
{
public:
    BlockingQueue()
    : mutex_(),
      notEmpty_(mutex_),
      queue_()
    {
    }

    void put(const T& x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify(); // TODO: move outside of lock
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        // always use a while-loop, due to spurious wakeup
        while (queue_.empty())
        {
            notEmpty_.wait();
        }
        assert(!queue_.empty());
        T front(queue_.front());
        queue_.pop_front();
        return front;
    }

    size_t size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

private:
    mutable MutexLock mutex_;
    Condition notEmpty_;
    std::deque<T>   queue_;
};

}
