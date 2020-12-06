#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

class EchoServer
{

private:
    EventLoop *_m_pLoop;
    TcpServer _m_tcpServer;

    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time);

public:
    void start();
    EchoServer(EventLoop* loop, InetAddress& listenAddr);
};
