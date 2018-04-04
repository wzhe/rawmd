/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <base/noncopyable.h>
#include <base/Condition.h>
#include <base/Mutex.h>
#include <base/Thread.h>
#include <base/Types.h>


#include <functional>
#include <vector>
#include <memory>
#include <deque>


namespace rawmd
{

class ThreadPool : rawmd::noncopyable
{
public:
    typedef std::function<void ()> Task;

    explicit ThreadPool(const string& name = string());
    ~ThreadPool();

    void start(int numThreads);
    void stop(); 

    void run(const Task& f);

private:
    void runInThread(); 
    Task take();

    MutexLock mutex_;
    Condition cond_;
    string name_;
    std::vector<std::unique_ptr<rawmd::Thread>> threads_;
    std::deque<Task> queue_;
    bool running_;
};

}
