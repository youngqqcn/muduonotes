#include <stdio.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>


using namespace muduo;
using namespace muduo::net;

class Server : noncopyable 
{
public:
    Server(EventLoop* loop, const InetAddress& addr, string name):
    _m_pLoop(loop),_m_server(_m_pLoop, addr, name)
    {
        _m_server.setMessageCallback(std::bind(&Server::onMessage, this, _1, _2, _3));
        _m_server.setConnectionCallback(std::bind(&Server::onConnection, this, _1));
    }

    void start()
    {
        _m_server.start();
    }

private:
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp tm)
    {
        LOG_INFO << "onMessage() ================";
    }

    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << "onConnection() ================";
    }


    EventLoop* _m_pLoop;
    TcpServer _m_server;
};





int main(int argc, char const *argv[])
{
    /* code */
    // printf("server\n");
    EventLoop loop;
    Server server(&loop, InetAddress(9000), "server");
    server.start();
    loop.loop();

    return 0;
}
