#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

// void onMessage(const TcpConnectionPtr& conn, Buffer* pBuf, Timestamp timestamp)
// {
//         if(pBuf->findCRLF())
//         {
//             conn->send("goooooooood bye\n");
//             conn->shutdown();
//         }
// }

int main()
{

    EventLoop loop;
    TcpServer server(&loop, InetAddress(9000), "finger05");

    server.setMessageCallback([](const TcpConnectionPtr &conn, Buffer *pBuf, Timestamp timestamp) {
        if (pBuf->findCRLF())
        {
            conn->send("goooooooood bye\n");
            conn->shutdown();
        }
    });
    server.start();
    loop.loop();
    return 0;
}