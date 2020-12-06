#include "echo.h"
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) 
{
    LOG_INFO << "pid = " << getpid();

    EventLoop  loop;
    InetAddress listenAddr(9000);

    EchoServer server(&loop, listenAddr);
    server.start();
    loop.loop();
    return 0;
}