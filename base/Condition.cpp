/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#include <base/Condition.h>

#include <errno.h>

// return true if time out, false otherwise.
bool rawmd::Condition::waitForSeconds(int seconds)
{
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += seconds;
    return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
}