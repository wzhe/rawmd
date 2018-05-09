/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once

#include <vector>
#include <iostream>

//the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

class MiniTestCase
{
public:
    MiniTestCase(const char* case_name) : nTestResult(1),testcase_name(case_name)
    {}

    //执行测试案例的方法
    virtual void Run() = 0;

    int nTestResult; //测试案例的执行结果
    const char* testcase_name;
};

class MiniUnitTest
{
public:
    //获取单例
    static MiniUnitTest* GetInstance();

    //注册测试案例
    MiniTestCase* RegisterTestCase(MiniTestCase* testcase);

    //执行测试
    int Run();

    MiniTestCase* CurrentTestCase;  //记录当前执行的测试案例
    int nTestResult;                //总的执行结果
    int nPassed;                    //通过案例数
    int nFailed;                    //失败案例数
protected:
    std::vector<MiniTestCase*> testcases_;
};


MiniUnitTest* MiniUnitTest::GetInstance()
{
    static MiniUnitTest instance;
    return &instance;
}

MiniTestCase* MiniUnitTest::RegisterTestCase(MiniTestCase* testcase)
{
    testcases_.push_back(testcase);
    return testcase;
}

int MiniUnitTest::Run()
{
    nTestResult = 1;
    for (std::vector<MiniTestCase*>::iterator it = testcases_.begin();
        it != testcases_.end(); ++it)
    {
        MiniTestCase* testcase = *it;
        CurrentTestCase = testcase;
        std::cout << GREEN << "======================================" << std::endl;
        std::cout << GREEN << "Run TestCase:" << testcase->testcase_name << std::endl;
        testcase->Run();
        std::cout << GREEN << "End TestCase:" << testcase->testcase_name << std::endl;
        if (testcase->nTestResult)
        {
            nPassed++;
        }
        else
        {
            nFailed++;
            nTestResult = 0;
        }
    }

    std::cout << GREEN << "======================================" << std::endl;
    std::cout << GREEN << "Total TestCase : " << nPassed + nFailed << std::endl;
    std::cout << GREEN << "Passed : " << nPassed << std::endl;
    std::cout << GREEN << "Failed : " << nFailed << std::endl;
    return nTestResult;
}

#define TESTCASE_NAME(testcase_name) \
    testcase_name##_TEST

#define MINI_TEST_(testcase_name) \
class TESTCASE_NAME(testcase_name) : public MiniTestCase \
{ \
public: \
    TESTCASE_NAME(testcase_name)(const char* case_name) : MiniTestCase(case_name){}; \
    virtual void Run(); \
private: \
    static MiniTestCase* const testcase_; \
}; \
\
MiniTestCase* const TESTCASE_NAME(testcase_name) \
    ::testcase_ = MiniUnitTest::GetInstance()->RegisterTestCase( \
        new TESTCASE_NAME(testcase_name)(#testcase_name)); \
void TESTCASE_NAME(testcase_name)::Run()

#define MTEST(testcase_name) \
    MINI_TEST_(testcase_name)

#define MRUN_ALL_TESTS() \
    MiniUnitTest::GetInstance()->Run();

#define EXPECT_EQ(m, n) \
    if (m != n) \
    { \
        MiniUnitTest::GetInstance()->CurrentTestCase->nTestResult = 0; \
        std::cout << RED << "Failed" << std::endl; \
        std::cout << RED << "Expect:" << m << std::endl; \
        std::cout << RED << "Actual:" << n << std::endl; \
    }

#define TEST_ASSERT(m) \
    if (!(m) \
    { \
        MiniUnitTest::GetInstance()->CurrentTestCase->nTestResult = 0; \
        std::cout << RED << "Failed" << std::endl; \
        std::cout << RED << "TEST_ASSERT:" << m << std::endl; \
    }

