#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;

int main(int argc, char const *argv[])
{
    /* code */
    EventLoop loop;
    TcpServer server(&loop, InetAddress(9000), "echo");

    server.setMessageCallback([](const TcpConnectionPtr& conn, Buffer* pBuf, Timestamp timestamp){
        // conn->connected();
        string msg( pBuf->retrieveAllAsString() );
        LOG_INFO << conn->peerAddress().toIpPort() << " -> " << msg;
        conn->send(msg);
    });

    server.start();
    loop.loop();
    
    return 0;
}

