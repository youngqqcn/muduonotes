#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
// #include <stdio.h>

using namespace muduo;
using namespace muduo::net;

int main(int argc, char const *argv[])
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(9000), "time");
    server.setConnectionCallback([](const TcpConnectionPtr &conn) {
        LOG_INFO << "time " << conn->peerAddress().toIpPort() << " "
                 << conn->localAddress().toIpPort() << " " << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            time_t now = ::time(NULL);
            int32_t tm32 = sockets::hostToNetwork32(static_cast<int32_t>(now));
            conn->send(&tm32, sizeof(tm32));
            conn->shutdown();
        }
    });
    server.start();
    loop.loop();

    return 0;
}
