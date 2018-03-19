#include <base/Exception.h>
#include <stdio.h>

class Bar
{
public:
  void test()
  {
    throw rawmd::Exception("oops");
  }
};

void foo()
{
  Bar b;
  b.test();
}

int main()
{
  try
  {
    foo();
  }
  catch (const rawmd::Exception& ex)
  {
    printf("reason:%s\n", ex.what());
    printf("stact trace:%s\n", ex.stackTrace());
  }
}
