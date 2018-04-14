# muduo base库学习笔记

## 前言

> 注：**个人水平有限，不能保证此文完全正确，如果错误，请不吝指出。** 
>
> 此文在观看《大并发服务器开发》视频时所做。
>
> 因此本文所讲基于muduo库0.9.1-beta版本,需要依赖cmake及boost.
> (rawmd不依赖boost,依赖c++11)
> sudo apt-get install cmake libboost-dev libboost-test-dev .
>
> 更新时间:<2018-03-12>
>
> 原创时间: <2018-03-12>

### 面向对象编程与基于对象编程

1. std::boost::function 

2. std::bootst::bind

## muduo::base中类关系及作用

1. copyable.h 一个空类，用来标识子类是值语义。

2. Types.h 提供字符串类的声明，及两个类型转换函数模板。

   1. implicit_cast (隐式转换)，使代码阅读时更明晰。
   2. down_cast (向下转型），基类指针转换为派生类指针，前提是基类指针指向派生类对象。

3. Timestamp.h/cc 提供封装好的时间类。

   - 继承copyable，值语义。
   - 继承boot::less_than_comparable\<T>,less_than_comparable：要求提供<，可自动实现>，<=，>=
   - BOOST_STATIC_ASSERT(sizeof(Timestamp) == sizeof(int64_t)); 编译期间的assert，在编译时就可以报错。
   - 打印64位整数使用 PRld64,  printf("%" PRld64 "\n", value_64); 可以做到跨平台（32和64位）。

4. Atomic.h 提供原子操作模板类。_延伸阅读：[无锁队列的实现](https://coolshell.cn/articles/8239.html)_

   1. 提供了AtomicInt32（用来标识线程数使用）和AtomicInt64（目前还未看出作用） 两个类。
   2. volatile关键字：确保本条指令不会因编译器的优化而省略，且要求每次直接读值。当要求使用volatile声明的变量的值时，系统总是重新从它所在的内存读取数据，而不是使用保存在寄存器中的备份。

5. Exception.h/cc 提供异常类

   1. backtrace 栈回溯，保存各个栈帧的地址。
   2. backtrace——symbols，根据地址，转成相应的函数符号。
   3. c++会在编译时会对函数做名字改编（name mangling）。
   4. abi::__cxa_demangle   对c++的程序做名字还原。

6. Thread.h/cc 线程类

   - Linux中进程与线程
     - Linux中每个**进**程有一个pid，类型是pid_t，由getpid()取得。
     - Linux下每个POSIX**线**程也有一个id，类型是pthread_t，由pthread_self()取得
     - POSIX**线**程id由线程库维护，其id空间是各个**进**程独立的。（即不同的**进**程中的**线**程可能有相同的id）。
     - Linux中的POSIX线程库实现的**线**程其实也是一个**进**程(LWP),只是该进程与主进程（启动线程的进程）共享一些资源而已，比如代码段，数据段等。
     - 有时候我们可能需要知道线程的真实pid。比如进程P1要向进程P2中的某个线程发送信号时，既不能使用P2的pid，更不能使用线程的pthread_id,而只能使用该线程的真实pid，称为tid。
     - 有一个函数gettid()可以得到tid，但glibc并没有实现该函数，只能通过 Linux的系统调用syscall来获取。
       - return syscall(SYS_gettid)

   1. 普通成员函数，隐含的第一个参数是 Thread*(this)，调用是时候遵循 thiscall约定，某一个寄存器会保护this指针，不能直接作为pthread_create的入口函数。需要定义一个static的静态成员函数，传递this指针来调用。
   2. static AtomicInt32 numCreated_; //用来记录创建的线程数，原子操作类型，在Thread类的构造中加1，析构上减1.
   3. __thread 关键字可以定义每个线程各个的变量。只能用来修饰基本类型（内置类型（与C兼容的原始类型）或无构造函数的类（结构体))，POD类型。 
   4. 将真实线程tid缓存起来，可以减少系统调用次数。
   5. namespace detail 内部使用的命名空间，对外不可见
   6. boost::is_same 用来判定两种类型是否是同一种类型
   7. pthread_atfork  (void (*prepare)(void), void(*parent)(void), void(*child)(void));
      - 调用fork时，内部创建子进程前在父进程中调用prepare
      - 内部创建子进程成功后，父进程会调用parent
      - 子进程会调用child
      - fork 可能是在主线程中调用，也可能是在子线程中调用
      - fork得到一个新进程，新进程只有一个执行序列，只有一个线程（调用fork的线程被继承下来）
      - 实际上，对于编写多线程程序来说，我们最好**不要再调用fork**
      - **不要编写多进程多线程程序**，要么用**多进程**要么用**多线程**

7. Mutex.h 提供锁

   1. MutexLocal类
      1. 继承自boot::noncopyable,不可拷贝的，对象语义
   2. MutexLockGuard类（更常用）
      1. 使用RAII技法（通过生存期来管理资源，在构造函数中申请资源，在析构函数中释放资源）封装的MutexLocal类

8. Condition.h/cc 条件变量类（Condition类）
      ```sequence
       participant Thread1
       participant mutex
       participant Thread2
       Thread1->mutex:锁住 mutex
       Note over Thread1: wait(等待条件成立)
       Thread2->mutex:锁住 mutex
       Note over Thread2: 更改条件(signal or broadcast)
       Note over Thread1: wait(条件成立)
       Thread2->mutex:解锁
       Thread1->mutex:解锁
      ```
      需要理解其中的wait函数，其实做了三件事
      1. 释放当前解锁mutex
      2. 判断条件是否成立，如不成立，则阻塞当前线程
      3. 条件成立，加锁mutex
9. CountDownLatch.h/cc 倒计时门闩类
   - 可以用于所有子线程等待主线程发起“起跑”
   - 可以用于主线程等待子线程初始化完毕才开始工作
   - mutbale关键字，使const函数可以修改mutable数据成员的状态。


10. BlockingQueue.h/BoundedBlockQueue.h  无界/有界缓冲区模板类（BlockingQueue/BoundedBlockQueue 模板类） （实际为**生产者->消费者**模型）

  ```sequence
  title:用有界队列来讲解生产者与消费者模型
  participant 生产者1
  participant 生产者2
  participant 队列
  participant 消费者1
  participant 消费者2

  生产者1->队列: 队列是否已满？
  Note right of 生产者1: 使用的是\nwait(条件变量)\n利用第8条\nCondition类。
  队列->生产者1: 列队未满
  生产者1->队列: 加锁
  生产者1->队列: 生产产品到队列中
  生产者1->队列: 解锁
  生产者1->消费者1: 队列非空
  生产者1->消费者2: 队列非空
  Note over 队列: 生产者工作流程已说明完毕\n下面说明消费者工作流程
  消费者1->队列:队列是否非空
  队列->生产者1: 列队非空
  消费者1->队列: 加锁
  消费者1->队列: 消费队列中的产品
  消费者1->队列: 解锁
  消费者1->生产者1: 队列未满
  消费者1->生产者2: 队列未满
  #基本流程已说明完毕，下面写一些混乱的状态
  Note over 队列: 基本流程已说明完毕\n下面随便写一些状态
  生产者2->队列: 队列是否已满？
  队列->生产者2: 队列未满
  生产者2->队列: 加锁
  生产者2->队列: 生产产品到队列中
  生产者2->队列: 解锁
  生产者1->队列: 队列是否已满？
  队列->生产者1: 队列已满
  生产者2->消费者1: 队列非空
  生产者2->消费者2: 队列非空
  生产者2->队列: 队列是否已满？
  队列->生产者2: 队列已满
  ```

  1. BoundedBlockingQueue中使用了 boost::circular_buffer（环形缓冲区）
    - 可以使用数组与求模运算自己实现环形缓冲区（boost中并不是这样实现的）

11. ThreadPoll.h/cc 线程池类（ThreadPool类）（本质上也是**生产者->消费者**模型）

    ```mermaid
    graph TB
    	subgraph ThreadPoll 
            Consumer["线程队列(消费者)"]-- "完成任务" -->任务队列
        end
        subgraph ExternalThreadPool
            Producer["外部线程(生产者)"]-- "提出任务" -->任务队列
        end
    ```

    1. muduo库中实现的是固定的线程池（即线程池中线程个数固定的），不需要考虑线程池中线程个数的动态伸缩。
    2. typedef boost::function\<void ()> Task;

12. Singleton.h 线程安全的单例模板类(Singleton模板类)

    1. 继承自boost::noncopyable，不可以被拷贝，对象语义。

    2. 只有instance()一个公有静态（public static）成员函数

    3. 通过pthread_once()来保证线程安全 及  单例。

    4. typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1]; //通过此语句要求T必须是完全类型。（只有前置声明的类是非完全类型）。此语句通过将数组下标设置为-1，来引发编译错误。

    5. 上一点在最新的编译器中编译时会报警告 [-Werror=unused-local-typedefs]，需要改为以下形式

       ```c++
       #if defined(__GNUC__)
           #define COMPILE_ASSERT_UNUSED_ATTRIBUTE __attribute__((unused))
       #else
           #define COMPILE_ASSERT_UNUSED_ATTRIBUTE /* nothing */
       #endif
       	 typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1]  COMPILE_ASSERT_UNUSED_ATTRIBUTE;
       ```

    6. ::atexit(destroy)  注册销毁函数，在程序结束时会自动去执行销毁函数。

13. ThreadLocal.h 线程特定数据模板类（ThreadLocal模板类），主要目的是提供__thread 的扩展

    - 在单线程程序中，我们经常使用“全局变量”以实现多个函数间共享数据

    - 在多线程环境下，由于数据空间是共享的，因此全局变量也为所有线程所共有

    - 但有时应用程序设计中有必要提供线程私有的全局变量，仅在某个线程中有效，但却可以跨多个函数访问

    - POSIX线程库通过维护一定的数据结构来解决这个问题，这些数据称为线程特定数据（Thread-specific Data,TSD)，也可程序线程本地存储(Thread-local storage)。

    - 对于POD类型的线程本地存储，可以使用__thread关键字

    - POSIX提供了4个函数

      - pthread_key_create  创建一个key，一旦一个线程创建了一个key，那么每一个线程都拥有了这个key。但可以为不同的线程指定不同的数据，通过下面的pthread_setspecific来实现。
      - pthread_key_delete 删除这个key，并非删除实际数据，实际数据的删除，需要在create  key时指定一个回调函数，由回调函数来销毁实际数据（在堆栈中）。
      - pthread_getspecific  获取特定数据。
      - pthread_setspecific  指定特定数据。

      |           |   线程0   |  线程...  |   线程n   |
      | :-------: | :-----: | :-----: | :-----: |
      |           | 线程其他信息  | 线程其他信息  | 线程其他信息  |
      |  pkey[0]  |   指针    |   指针    |   指针    |
      |  pkey[1]  |   指针    |   指针    |   指针    |
      | pkey[...] |   ...   |   ...   |   ...   |
      | pkey[127] |   指针    |   指针    |   指针    |
      |  系统数据结构   |  ————   |  ————   |  ————   |
      | 线程分析的内存区域 |  ————   |  ————   |  ————   |
      |   堆栈地址    | 上面指针所指向 | 上面指针所指向 | 上面指针所指向 |
      |   堆栈地址    |  的实际数据  |  的实际数据  |  的实际数据  |

    - 测试程序SingletonThreadLocal_test.cc中的代码有绕，需要慢慢推敲，弄清楚什么是单例的，什么是线程特定数据，就明白STL是什么了。

14. ThreadLocalSingleton.h 线程本地单例模板类（ThreadLocalSingleton模板类）

    1. 不需要线程安全，每一个线程都有一个T类型的单例对象，
    2. 继承自boost::noncopyable，不可以被拷贝，对象语义。
    3. 综合使用了__thread 与 TSD两种线程本地存储技术
       1. 存储T类型的指针（T*），因为指针是POD类型，因此使用__thread关键字。
       2. 利用RAII技法，嵌套一个Deleter类，来实现TSD。

15. Logging.h/cc  LogStream.h/cc

    1. 日志的作用：

       - 开发过程中：
         - 调试错误 （包括编译错误 、运行错误、逻辑错误）
         - 更好的理解程序
       - 运行过程中：
         - 诊断系统故障并处理
         - 记录系统运行状态

    2. 日志级别：

       1. TRACE 指出比DEBUG粒度更**细**的一些信息事件（开发过程中使用）
       2. DEBUG 指出对调试程序非常有帮助的**细粒度**信息事件（开发过程中使用)
       3. INFO 指出在**粗粒度**级别上突出强调程序的运行过程的信息事件（muduo库中默认此级别）
       4. WARN 系统能正常运行，但可能会出现潜在错误的情形
       5. ERROR 指出虽然发生错误 ，但仍然不影响系统的继续运行
       6. FATAL 指出每一个严重的错误事件，并导致程序退出

    3. Log类使用时序图

       Logger ==> lmpl ==> LogStream ==> operator<<() ==> FixedBuffer ==> g_output ==> g_flush

       Logger 日志类外部封装（负责日志的级别） ==> lmpl与LogStream 日志类内部的实现（负责日志的格式化）==> operator<<() 负责日志类的输出 ==> FixedBuffer 日志输出的缓冲区 ==> g_output 指定日志输出到不同的设备 ==> g_flush 完成输出。

       ​

       ```sequence
       title:Logger类简化模型
       participant user
       participant Logger
       participant LogStream

       user->Logger: 1：stream()
       Logger->LogStream: 2:operator <<()
       ```

    4. 注意在::fopen("/tmp/muduo.log","ae")中参数e，

       >    e (since glibc 2.7)
       >
       >    ​          Open the file with the **O_CLOEXEC**flag.  See open(2) for more information.  This flag is ignored for **fdopen()**.

    - LogStream类中

      - FixedBuffer类 缓冲区类，只接收非类型(int SIZE)参数的模板类，
        1. 继承自boost::noncopyable,不可以拷贝。
        2. const常量 根据google的规范，使用k开头，如下：
          ```c++
            const int kSmallBuffer = 4000;
            const int kLargeBuffer = 4000*1000;
          ```
        3. 下图是FixedBuffer类在内存中的示意图

          |                        | 大小为SIZE的一块内存 |
          | ---------------------- | ------------ |
          | data_(起始地址)-->         | 0            |
          |                        | 1            |
          | cur_(当前地址,标识已使用的内存)--> | 2            |
          |                        | ...          |
          |                        | SIZE-2       |
          | data_+SIZE-1-->        | SIZE-1       |

      - Fmt类
        1. 断言T是算术类型
           ```c++
           BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);
           ```

        2. bzero函数

           ```c++
           ::bzero(data_, sizeof data_);
           //等价于
           memset(data_, 0, sizeof data_);
           ```

           bzero函数，包含于\<strings.h\>中，是**线程安全**的，是POSIX标准函数，属于BSD系函数，没有在ANSI中定义，Windows平台与Mac平台都将不包含将函数，Linux下GCC编译器支持。[(信息来源)](http://blog.csdn.net/hello_world6/article/details/53495270)

           不太理解这里为什么使用了bzero函数，只有这篇文章[bzero & memset置零的性能比较](http://blog.csdn.net/taolinke/article/details/4487364)中讲了下在数组较小时，bzero效率比memset要高。另一个优点是使代码易读吧。另外man手册中说这个函数**已经过时**了。

           >       ​	The **bzero()** function is deprecated (marked as LEGACY in POSIX.1-2001); use **memset**(3) in new programs.  POSIX.1-2008 removes the  specification  of **bzero()**.   The  **bzero()**  function  first  appeared in 4.3BSD.

      - BOOST_AUTO_TEST_CASE

        - boost自动测试框架(sudo apt-get install libboost-test-dev)
        - std::numeric_limits\<T\>::max()  std::numeric_limits\<T\>::min() 

      - StringPiece类 ----google 高效字符串的实现

        - 用来实现高效的字符串传递 这里可以是const char* 也可以是std::string也可以，并且这里不涉及内存拷贝

        - 在STL中为了提供通用的操作而又不损失效率，我们用到了一种特殊的技巧，叫traits编程技巧。具体的来说，traits就是通过定义一些结构体或类，并利用模板特化和偏特化的能力，给类型赋予一些特性，这些特性根据类型的 不同而异。在程序设计中可以使用这些traits来判断一个类型的一些特性，引发C++的函数重载机制，实现同一种操作因类型不同而异的效果。_延申阅读：[C++ Template](http://vdisk.weibo.com/s/dikrSBqw8e7da)(繁体中文版)_

          ```c++
          // 这里对__type_traits进行特化，给StringPiece一些特性
          #ifdef HAVE_TYPE_TRAITS
          // This makes vector<StringPiece> really fast for some STL implementations
          template<> struct __type_traits<muduo::StringPiece> {
            typedef __true_type    has_trivial_default_constructor;
            typedef __true_type    has_trivial_copy_constructor;
            typedef __true_type    has_trivial_assignment_operator;
            typedef __true_type    has_trivial_destructor;
            typedef __true_type    is_POD_type;
          };
          #endif
          ```

      - LogFile类

        - 日志滚动条件
          - 文件大小（例如：每满1G就换一个文件）
          - 时间 （例如，每一小时写一个文件）
        - gmtime与gmtime_r  
          - gmtime不是线程安全的，返回的是一个tm指针，但是另一个线程可能也在调用gmtime，因此指针所指向的内存地址可能发生了变化。
          - gmtime_r是线程安全的，虽然也返回了一个tm指针，但同时将结果保存在result中。


## cmake的使用	

1. muduo库中使用到的编译选项
   - -Wall 		//大部分警告
   - -Wextra         // 一些额外的警告
   - -Werror          //当出现警告时转为错误，停止编译
   - -Wconversion  //一些要能改变值的陷式转换，给出警告
   - -Wno-unused-parameter //函数中出现未使用的参数,不给出警告
   - -Wold-style-case   //C风格的转换，给出警告
   - -Woverloaded-virtual  //如果函数的声明隐藏住了基类的虚函数，就给警告
   - -Wpointer-arith     //对函数指针或者void*类型的指针进行算术操作时给出警告
   - -Wshadow    //当一个局部变量遮盖住了另一个局部变量，或者全局变量时，给出警告
   - -Wwrite-strings   //替换字符串常量的类型时const char[length],因此，把这样的地址复制给non-const char* 指针将产生警告。这些警告能够帮助你在编译期间发现企图写入字符串常量的代码
   - -march=native  //指定cpu体系结构为本地平台

# muduo库中还不理解的内容

1. 无锁队列的实现
2. **O_CLOEXEC** 的详细解释与具体作用
3. boost自动测试框架











