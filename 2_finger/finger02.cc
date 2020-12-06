#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;

int main(int argc, char const *argv[])
{

    EventLoop loop;
    InetAddress listenAddr("127.0.0.1", 9000);
    TcpServer server(&loop, listenAddr, "Finger02", TcpServer::kReusePort);
    server.start();
    LOG_INFO << "start " << server.name() << "  at  " << listenAddr.toIpPort();
    loop.loop();
    return 0;
}
