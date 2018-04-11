#include <base/ThreadLocal.h>
#include <base/CurrentThread.h>
#include <base/Thread.h>
#include <base/noncopyable.h>

#include <stdio.h>

class Test : rawmd::noncopyable
{
public:
	Test()
	{
		printf("tid=%d, constructing %p \n", rawmd::CurrentThread::tid(), this);
	}

	~Test()
	{
		printf("tid=%d, destructing %p %s \n", rawmd::CurrentThread::tid(), this, name_.c_str());
	}

	const std::string& name() const { return name_;}
	void setName(const std::string& n) { name_ = n; }

private:
	std::string name_;
};

rawmd::ThreadLocal<Test> testObj1;
rawmd::ThreadLocal<Test> testObj2;

void print()
{
  printf("tid=%d, obj1 %p name=%s\n", rawmd::CurrentThread::tid(), &testObj1.value(), testObj1.value().name().c_str());
  printf("tid=%d, obj2 %p name=%s\n", rawmd::CurrentThread::tid(), &testObj2.value(), testObj1.value().name().c_str());
}

void threadFunc()
{
	print();
	testObj1.value().setName("changed 1");
	testObj2.value().setName("changed 42");
	print();
}

int main()
{
	testObj1.value().setName("main one");
	print();
	rawmd::Thread t1(threadFunc);
	t1.start();
	t1.join();
	testObj2.value().setName("main two");
	print();

	pthread_exit(0);
}
