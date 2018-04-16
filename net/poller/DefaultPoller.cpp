/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#include <net/Poller.h>
#include <net/poller/PollPoller.h>
#include <net/poller/EPollPoller.h>

#include <stdlib.h>

using namespace rawmd::net;

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
  if (::getenv("RAWMD_USE_POLL"))
  {
    return new PollPoller(loop);
  }
  else
  {
    return new EPollPoller(loop);
  }
}
