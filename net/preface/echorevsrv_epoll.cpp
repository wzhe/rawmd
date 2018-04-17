#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>     //inet_ntoa
#include <arpa/inet.h>      //inet_ntoa
#include <fcntl.h>          //O_RDONLY O_CLOEXEC


#include <sys/epoll.h>

#include <unistd.h>         //close
#include <signal.h>
#include <errno.h>
#include <string.h>         //memset


#include <algorithm>
#include <iostream>
#include <vector>


#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

typedef std::vector<struct epoll_event> EventList;

int main(void)
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    int idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);
    int listenfd;

    if ((listenfd = socket(PF_INET,SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt");
    if (bind(listenfd, reinterpret_cast<struct sockaddr*>(&servaddr), sizeof(servaddr)) < 0)
        ERR_EXIT("bind");
    if (listen(listenfd, SOMAXCONN) < 0)
        ERR_EXIT("listen");

    std::vector<int> clients;
    int epollfd = epoll_create1(EPOLL_CLOEXEC);         //创建epoll
    if (epollfd == -1)
        ERR_EXIT("epoll_create1");

    struct epoll_event event;
    event.data.fd = listenfd;
    event.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event) == -1)
        ERR_EXIT("epoll_ctl");

    EventList events(16);
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int connfd;

    int nready;

    while (true)
    {
        nready = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), -1);
        if (nready == -1)
        {
            if (errno == EINTR)
                continue;

            ERR_EXIT("epoll_wait");
        }

        if (nready == 0) //nothing happened
            continue;

        if (nready == static_cast<int>(events.size()))
            events.resize(events.size() * 2);

        for (int i = 0; i < nready; ++i)
        {
            if (events[i].data.fd == listenfd)
            {
                peerlen = sizeof(peeraddr);
                connfd = ::accept4(listenfd, reinterpret_cast<struct sockaddr*>(&peeraddr),
                            &peerlen, SOCK_NONBLOCK | SOCK_CLOEXEC);

                if (connfd == -1)
                {
                    if (errno == EMFILE)
                    {
                        close(idlefd);
                        idlefd = accept(listenfd, NULL, NULL);
                        close(idlefd);
                        idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);
                        continue;
                    }
                    else
                        ERR_EXIT("accept4");
                }

                std::cout << "client:" << connfd << " ip=" << inet_ntoa(peeraddr.sin_addr) <<
                    " port=" << ntohs(peeraddr.sin_port) << std::endl;

                clients.push_back(connfd);

                event.data.fd = connfd;
                event.events = EPOLLIN;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event) == -1)
                    ERR_EXIT("epoll_ctl");
            }
            else if (events[i].events & EPOLLIN)
            {
                connfd = events[i].data.fd;
                if (connfd < 0)
                    continue;

                char buf[1024] = {0};
                char rbuf[1024] = {0};
                ssize_t ret = read(connfd, buf, sizeof(buf));
                if (ret == -1)
                    ERR_EXIT("read");
                if (ret == 0)
                {//对端关闭了
                    std::cout << "client(" << connfd << ") close" << std::endl;
                    close(connfd);
                    event = events[i];
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, connfd, &event);
                    clients.erase(std::remove(clients.begin(), clients.end(), connfd), clients.end());
                    continue;
                }
                //std::cout << "ret:" << ret <<" strlen(buf):" << strlen(buf) << std::endl;
                std::cout << buf;
                for(int j = 0; j < ret-1; ++j)
                {
                    rbuf[j] = buf[ret-2-j];
                }
                rbuf[ret-1] = buf[ret-1];   //LF
                std::cout << rbuf;
                write(connfd, rbuf, strlen(rbuf));
            }
        }
    }
    return 0;
}
