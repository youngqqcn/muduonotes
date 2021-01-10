#include <stdio.h>

#include "echo.cc"

int main(int argc, char const *argv[])
{
    /* code */
    EventLoop loop;
    EchoServer server(&loop, InetAddress(9000), "echo", 5);
    server.start();
    loop.loop();
    
    printf("hello\n");
    return 0;
}
