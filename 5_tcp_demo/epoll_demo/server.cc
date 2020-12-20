#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include<netinet/in.h> 
#include <vector>
#include <thread>
#include <arpa/inet.h>

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

    // int clientFds[];
    vector<int> vctClientFds;
    vector<thread> vctThreads;

    sockaddr_in cliAddr;
    socklen_t cliSckkLen = sizeof(cliAddr);
    memset(&cliAddr, 0, sizeof(sizeof(cliAddr)));
    while(1)
    {
        int cliFd = ::accept(fd, (struct sockaddr* )(&cliAddr), &cliSckkLen);
        if(cliFd < 0)
        {
            perror("error accept\n");
            continue;
        }

        vctClientFds.push_back(cliFd);

        vctThreads.emplace_back([cliFd, cliAddr](){
            char buf[1024] = {0} ;
            while(1)
            {
                memset(buf, 0, sizeof(buf));
                int ret = recv(cliFd, buf, sizeof(buf), 0);
                if(ret < 0)
                {
                    printf(" recv error , socket fd %d\n", cliFd);
                    close(cliFd);
                    return;
                }
                else if(0 == ret)
                {
                    printf(" client : %s close socket\n ", inet_ntoa( cliAddr.sin_addr));
                    close(cliFd);
                    return;
                }
                printf("%s %d bytes data, date is : %s \n",  inet_ntoa( cliAddr.sin_addr), ret , buf);
            }
        });
    }

    return 0;
}
