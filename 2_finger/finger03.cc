#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

void onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        LOG_INFO << conn->peerAddress().toIpPort() << "  connected ";
        conn->shutdown();
    }
}

int main(int argc, char const *argv[])
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(9000), "Finger03");
    server.setConnectionCallback(onConnection);
    server.start();
    loop.loop();

    return 0;
}
