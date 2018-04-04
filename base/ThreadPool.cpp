/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#include <base/ThreadPool.h>
#include <base/Exception.h>

#include <functional>
#include <assert.h>
#include <stdio.h>
#include <algorithm>

using namespace rawmd;

ThreadPool::ThreadPool(const string& name)
  : mutex_(),
  cond_(mutex_),
  name_(name),
  running_(false)
{
}

ThreadPool::~ThreadPool()
{
  if (running_)
  {
    stop();
  }
}

void ThreadPool::start(int numThreads)
{
  assert(threads_.empty());
  running_ = true;
  threads_.reserve(numThreads);
  for (int i = 0; i < numThreads; ++i)
  {
    char id[32];
    snprintf(id, sizeof id, "%d", i);
    threads_.push_back(static_cast<std::unique_ptr<rawmd::Thread>>(new rawmd::Thread(
            std::bind(&ThreadPool::runInThread, this), name_+id)));
    threads_[i]->start();
  }
}

void ThreadPool::stop()
{
  {
    MutexLockGuard lock(mutex_);
    running_ = false;
    cond_.notifyAll();
  }
  for_each(threads_.begin(),
      threads_.end(),
      std::bind(&rawmd::Thread::join, std::placeholders::_1));
}

void ThreadPool::run(const Task& task)
{
  if (threads_.empty())
  {
    task();
  }
  else
  {
    MutexLockGuard lock(mutex_);
    queue_.push_back(task);
    cond_.notify();
  }
}


ThreadPool::Task ThreadPool::take()
{
  MutexLockGuard lock(mutex_);
  while (queue_.empty() && running_)
  {
    cond_.wait();
  }
  Task task;
  if (!queue_.empty())
  {
    task = queue_.front();
    queue_.pop_front();
  }
  return task;
}

void ThreadPool::runInThread()
{
  try
  {
    while (running_)
    {
      Task task(take());
      if (task)
      {
        task();
      }
    }
  }
  catch (const Exception& ex)
  {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
    abort();
  }
  catch (const std::exception& ex)
  {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    abort();
  }
  catch (...)
  {
    fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
    throw; // rethrow
  }
}


