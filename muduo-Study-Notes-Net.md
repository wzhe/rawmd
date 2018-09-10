# muduo net库学习笔记

## 前言

> 注：**个人水平有限，不能保证此文完全正确，如果错误，请不吝指出。** 
>
> 此文在观看《大并发服务器开发》视频时所做。
>
> 因此本文所讲基于muduo库0.9.1-beta版本,需要依赖cmake及boost.
> (rawmd不依赖boost,依赖c++11)
> sudo apt-get install cmake libboost-dev libboost-test-dev .
>
> 更新时间:<2018-04-14>
>
> 原创时间: <2018-04-14>

### epoll/poll/select


####poll

函数原型：
```c++
	#include <poll.h>
	int poll(struct pollfd *fds, nfds_t nfds, int timeout);

	struct pollfd{
		int fd;			// file descriptor
		short events;	// requested events
		short revents;	// returned events
	};
```
poll 的使用流程如下图，相关代码在[echosrv_poll.cpp](./net/preface/echosrv_poll.cpp)
```flow
st=>start: start
oppoll=>operation: poll
oplisten=>operation: 处理监听套接字
opconnect=>operation: 处理已连接套接字

st->oppoll->oplisten->opconnect(left)->oppoll
```

####epoll

函数原型：
```c++
	#include <sys/epoll.h>
	int epoll_create(int size);
	int epoll_create1(int size);
	
	int epoll_ctl(int epfd, int op, int fd, struct event *event);
	int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
	
	typedef union epoll_data{
      void *ptr;
      int  fd;
      uint32_t u32;
      uint64_t u64;
	}epoll_data_t;

	struct epoll_event{
		uint32_t	events;	// Epoll events
		epoll_data_t data;	// User data variable
	};
```

epoll 的使用流程如下图，相关代码在[echosrv_epoll.cpp](./net/preface/echosrv_epoll.cpp)
```flow
st=>start: start
opepoll=>operation: epoll_wait
oplisten=>operation: 处理监听套接字
opconnect=>operation: 处理已连接套接字

st->opepoll->oplisten->opconnect(left)->opepoll
```

epoll 有两种触发模式
	1 电平触发(Level-Triggered)
	2 边沿触发(Edge-Triggered)
电平触发模式就和poll一样，大多数情况下使用电平触发。这里只是简单地提一下（主要是我根本就不会^_^)

**编程小技巧**：当accept(2)返回EMFILE时的处理：
>准备一个空闲的文件描述符。遇到这种情况，先关闭这个空闲文件，获得一个文件描述符名额;再accept(2)拿到socket连接的文件描述符；随后立刻close(2)，这样就优雅地断开了与客户端的连接；最后重新打开空闲文件，把“坑”填上，以备再次出现这种情况时使用。

####epoll/poll/select比较

|            | 原理                                       |
| ---------- | ---------------------------------------- |
| **select** | select本质上是通过设置或者检查存放fd标志位的数据结构来进行下一步处理。这样所带来的缺点是：\n (1) 单个进程可监视的fd数量被限制(2) 需要维护一个用来存放大量fd的数据结构，这样会使得用户空间和内核空间在传递该结构时复制开销大 (3) 对socket进行扫描时是线性扫描 |
| **poll**   | poll本质上和select没有区别，它将用户传入的数组拷贝到内核空间，然后查询每个fd对应的设备状态，如果设备就绪则在设备等待队列中加入一项并继续遍历，如果遍历完所有fd后没有发现就绪设备，则挂起当前进程，直到设备就绪或者主动超时，被唤醒后它又要再次遍历fd。这个过程经历了多次无谓的遍历。 |
| **epoll**  | 在前面说到的复制问题上，epoll使用mmap减少复制开销。还有一个特点是，epoll使用“事件”的就绪通知方式，通过epoll_ctl注册fd，一旦该fd就绪，内核就会采用类似callback的回调机制来激活该fd，epoll_wait便可以收到通知。 |



|            | 一个进程所能打开的最大连接数                           |
| ---------- | ---------------------------------------- |
| **select** | 单个进程所能打开的最大连接数有FD_SETSIZE宏定义，其大小是32个整数的大小（在32位的机器上，大小就是32*32，同理64位机器上FD_SETSIZE为32*64），当然我们可以对进行修改，然后重新编译内核，但是性能可能会受到影响，这需要进一步的测试 |
| **poll**   | poll本质上和select没有区别，但是它没有最大连接数的限制，原因是它是基于链表来存储的。限制和epoll一样 |
| **epoll**  | 和poll一样。虽然连接数有上限，但是很大，1G内存的机器上可以打开10万左右的连接，2G内存的机器可以打开20万左右的连接 |



|            | 消息传递防止                   |
| ---------- | ------------------------ |
| **select** | 内核需要将消息传递到用户空间，都需要内核拷贝动作 |
| **poll**   | 内核需要将消息传递到用户空间，都需要内核拷贝动作 |
| **epoll**  | epoll通过内核和用户空间共享一块内存来实现的 |



|            | FD(file descriptor)剧增后带来的IO效率问题          |
| ---------- | ---------------------------------------- |
| **select** | 因为每次调用时都会对连接进行线性遍历，所以随着FD的增加会造成遍历速度慢的“线性下降性能问题”。 |
| **poll**   | 因为每次调用时都会对连接进行线性遍历，所以随着FD的增加会造成遍历速度慢的“线性下降性能问题”。 |
| **epoll**  | 因为epoll内核中实现是根据每个fd上的callback函数来实现的，只有活跃的socket才会主动调用callback，所以在活跃socket较少的情况下，使用epoll没有前面两者的线性下降的性能问题，但是所有socket都很活跃的情况下，可能会有性能问题。 |





## muduo net库

### EventLoop

1. one loop per thread 每个线程最多只能有一个EventLoop对象。EventLoop对象构造时会检查当前线程是否已经创建了其他EventLoop对象，如果已经创建，则终止程序(LOG_FATAL)
2. EventLoop 构造函数会记住本对象所属线程(threadId_)。
3. 创建EventLoop对象的线程称为IO线程，其功能是运行事件循环(EventLoop::loop)

### Channel
1. Channel是selectable IO channel, 负责注册与响应IO事件，它不拥有file descriptor.
2. Channel是Acceptor、 Connector、 EventLoop、TimerQueue、TcpConnection的成员，生命期由后者控制。

