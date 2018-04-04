/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/circularbuffer.h>
#include <base/noncopyable.h>
#include <base/Condition.h>
#include <base/Mutex.h>

#include <assert.h>

namespace rawmd
{

template<typename T>
class BoundedBlockingQueue : rawmd::noncopyable
{
public:
    BoundedBlockingQueue(int maxSize)
        : mutex_(),
          notEmpty_(mutex_),
          notFull_(mutex_),
          queue_(maxSize)
    {
    }

    void put(const T& x)
    {
        MutexLockGuard lock(mutex_);
        while(queue_.full())
        {
            notFull_.wait();
        }
        assert(!queue_.full());
        queue_.push_back(x);
        notEmpty_.notify(); // TODO: move outside of lock
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        while(queue_.empty())
        {
            notEmpty_.wait();
        }
        assert(!queue_.empty());
        T front(queue_.front());
        queue_.pop_front();
        notFull_.notify(); // TODO: move outside of lock
        return front;
    }

    bool empty() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.empty();
    }

    bool full() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.full();
    }

    size_t size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

    size_t capacity() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.capacity();
    }
private:
    mutable MutexLock                   mutex_;
    Condition                   notEmpty_;
    Condition                   notFull_;
    rawmd::circular_buffer<T>    queue_;
};

}
