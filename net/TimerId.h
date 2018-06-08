/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/copyable.h>

namespace rawmd
{
namespace net
{

class Timer;

///
/// An opaque identifier, for canceling Timer.
///
class TimerId : public rawmd::copyable
{
    public:
        TimerId()
            : timer_(NULL),
            sequence_(0)
    {
    }

        TimerId(Timer* timer, int64_t seq)
            : timer_(timer),
            sequence_(seq)
    {
    }

        // default copy-ctor, dtor and assignment are okay

        friend class TimerQueue;

    private:
        Timer* timer_;
        int64_t sequence_;
};

}
}

