#include <muduo/net/EventLoop.h>



int main(int argc, char const *argv[])
{
    muduo::net::EventLoop loop;
    loop.loop();
    return 0;
}
