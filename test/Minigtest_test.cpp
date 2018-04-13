#include "Minigtest.h"

int Foo(int a, int b)
{
    return a + b;
}

MTEST(FooTest_PassDemo)
{
    EXPECT_EQ(3, Foo(1, 2));
    EXPECT_EQ(2, Foo(1, 1));
}

MTEST(FooTest_FailDemo)
{
    EXPECT_EQ(4, Foo(1, 2));
    EXPECT_EQ(2, Foo(1, 2));
}

int main(int argc, char* argv[])
{
    return MRUN_ALL_TESTS();
}