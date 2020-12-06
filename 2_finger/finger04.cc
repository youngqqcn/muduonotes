#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

void onMessage( const TcpConnectionPtr& conn, Buffer* pBuf, Timestamp timestamp)
{
    const char *pName = pBuf->findCRLF();
    if(nullptr != pName)
    {
        LOG_INFO << "got name "  << pBuf->retrieveAllAsString();
        conn->shutdown();
    }

}

int main(int argc, char const *argv[])
{
    
    EventLoop loop;
    TcpServer server(&loop, InetAddress(9000), "finger04");
    server.setMessageCallback(onMessage);
    server.start();
    loop.loop();

    return 0;
}
