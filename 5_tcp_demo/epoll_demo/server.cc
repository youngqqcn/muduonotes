#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include<netinet/in.h> 
#include <vector>
#include <thread>
#include <arpa/inet.h>


 #include <sys/epoll.h>  // epoll

using namespace std;


int main(int argc, char const *argv[])
{

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("create socket failed\n");
        return 0;
    }

    // struct sockaddr_un addr;
    // addr.sun

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);
    socklen_t  len = sizeof(sockaddr);
    int opt = 1;
    setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) );
    int iRet = bind(fd, (const struct sockaddr *)&addr, len);
    if(iRet < 0)
    {
        // perror("bind error\n");
        printf("bind error\n");
        return 0;
    }

    if(listen(fd, 1000)  < 0)
    {
        // perror("listen error\n");
        printf("listen error\n");
        return 0;
    }


    int epfd = epoll_create(1000);
    if(epfd < 0)
    {
        printf("epoll create error\n");
        return 0;
    }

    struct epoll_event ev;
    ev.data.fd = fd;

    // EPOLLET 边缘触发, 一旦有数据到达就触发,只触发一次; 
    // EPOLLLT  水平触发 , 只要有数据未被处理, 就一直通知
    ev.events = EPOLLIN | EPOLLET; 

    if(0 > epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev))
    {
        printf("epoll_ctl error\n");
        return 0;
    }

    



    // int clientFds[];
    vector<int> vctClientFds;
    vector<thread> vctThreads;

    char recvBuf[1024] = {0} ;
    while(1)
    {
        struct epoll_event events[1000];
        int ret = epoll_wait(epfd, events, sizeof(events)/sizeof(events[0]) , -1);
        if(0 > ret)
        {
            printf("epoll_wait error\n");
            return 0;
        }
        else if(0 == ret )
        {
            printf("timeout\n");
            return 0;
        }

        // 便利event
        for(int i = 0; i < ret; i++)
        {
            struct epoll_event evt;
            memcpy(&evt,  &events[i], sizeof(epoll_event));

            if( (evt.events & EPOLLERR) || (evt.events & EPOLLHUP) || !(evt.events & EPOLLIN)  )
            {
                printf("epoll error");
                close(evt.data.fd);
                return 0;
            }

            // 新的连接
            if(evt.data.fd == fd) 
            {
                struct sockaddr_in cliAddr;
                socklen_t cliSckkLen = sizeof(cliAddr);
                int cliFd = ::accept(fd, (struct sockaddr* )(&cliAddr), &cliSckkLen);
                if(cliFd < 0)
                {
                    perror("error accept\n");
                    continue;
                }
                struct epoll_event event;
                event.data.fd = cliFd;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cliFd, &event);
                vctClientFds.push_back(cliFd);

                // printf("accepted connection from %s, port : %d\n", inet_ntoa( cliAddr.sin_addr), ntohl(cliAddr.sin_port));
                printf("accepted connection from %s, port : %d\n", inet_ntoa( cliAddr.sin_addr), ntohs(cliAddr.sin_port));
            }
            else if( evt.events & EPOLLIN ) // 数据
            {
                memset(recvBuf, 0, sizeof(recvBuf));
                int clifd = evt.data.fd;
                int ret = recv(clifd, recvBuf, sizeof(recvBuf), 0);
                if(ret < 0)
                {
                    printf(" recv error , socket fd %d\n", clifd);
                    close(clifd);
                }
                else if(0 == ret)
                {
                    printf("client close socket fd\n");
                    close(clifd);
                }
                else
                {
                    printf("client socket fd: %d , send %d bytes data, date is : %s \n", evt.data.fd , ret , recvBuf);
                }
                
            }

        }



        
    }

    return 0;
}
