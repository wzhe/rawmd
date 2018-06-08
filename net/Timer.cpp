/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#include <net/Timer.h>

using namespace rawmd;
using namespace rawmd::net;

AtomicInt64 Timer::s_numCreated_;

void Timer::restart(Timestamp now)
{
  if (repeat_)
  {
    // 重新计算下一个超时时刻
    expiration_ = addTime(now, interval_);
  }
  else
  {
    expiration_ = Timestamp::invalid();
  }
}
