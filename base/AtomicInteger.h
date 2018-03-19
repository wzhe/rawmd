/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/noncopyable.h>
#include <stdint.h>

namespace rawmd
{

namespace detail
{

template<typename T>
class AtomicIntegerT : rawmd::noncopyable
{
public:
    AtomicIntegerT() : value_(0)
    {
    }

    T get()
    {
        // gcc >= 4.7
        return __atomic_load_n(&value_, __ATOMIC_SEQ_CST);
    }

    T getAndSet(T newValue)
    {
        //  gcc >= 4.7
        return  __atomic_exchange_n(&value_, newValue, __ATOMIC_SEQ_CST);
    }

    T getAndAdd(T x)
    {
        // gcc >= 4.7
        return __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST);
    }

    T addAndGet(T x)
    {
        return getAndAdd(x) + x;
    }

    T incrementAndGet()
    {
        return addAndGet(1);
    }

    T decrementAndGet()
    {
        return addAndGet(-1);
    }

    void add(T x)
    {
        getAndAdd(x);
    }

    void increment()
    {
        incrementAndGet();
    }

    void decrement()
    {
        decrementAndGet();
    }

private:
    volatile T value_;
};
}

typedef detail::AtomicIntegerT<int32_t> AtomicInt32;
typedef detail::AtomicIntegerT<int64_t> AtomicInt64;

}
