#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <muduo/net/TcpServer.h>


using namespace muduo;
using namespace muduo::net;

int main(int argc, char const *argv[])
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(9000), "discard");
    server.setMessageCallback([](const TcpConnectionPtr& conn, Buffer *pBuf, Timestamp timestamp){

        // 丢弃所有收到的数据
        string msg ( pBuf->retrieveAllAsString() );
        LOG_INFO << conn->name() << " received: " << msg;
    } );
    server.start();
    loop.loop();

    return 0;
}


