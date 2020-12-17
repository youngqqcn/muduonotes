#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
// #include <stdio.h>

using namespace muduo;
using namespace muduo::net;

int main(int argc, char const *argv[])
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(9000), "daytime");
    server.setConnectionCallback([](const TcpConnectionPtr &conn) {
        LOG_INFO << "daytime " << conn->peerAddress().toIpPort() << " "
                 << conn->localAddress().toIpPort() << " " << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            string strTime = Timestamp::now().toFormattedString(false);
            conn->send(strTime + "\n");
            conn->shutdown();
        }
    });
    server.start();
    loop.loop();

    printf("hello\n");
    return 0;
}
