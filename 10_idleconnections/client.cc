#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>
#include <muduo/base/Logging.h>
#include <muduo/base/noncopyable.h>
#include <functional>

using namespace muduo;
using namespace muduo::net;

class MyEchoClient : noncopyable
{

public:
    MyEchoClient(EventLoop *ploop, const InetAddress &addr)
        : _m_pLoop(ploop),
          _m_client(ploop, addr, "client")
    {
        _m_client.setConnectionCallback(std::bind(&MyEchoClient::onConnection, this, _1));
        _m_client.setMessageCallback(std::bind(&MyEchoClient::onMessage, this, _1, _2, _3));
    }

    void connect()
    {
        _m_client.connect();
    }

private:
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp tm)
    {
        LOG_INFO << "=============onMessage";
    }

    void onConnection(const TcpConnectionPtr &conn)
    {
        if(conn->connected())
        {
            LOG_INFO << "=============onConnection";
        }
        else
        {
            LOG_INFO << "conn closed , shut it down" ;
            conn->shutdown();
            // LOG_INFO << 
        }
    }

private:
    TcpClient _m_client;
    EventLoop *_m_pLoop;
};

int main(int argc, char const *argv[])
{
    /* code */
    // printf("hello\n");
    EventLoop loop;
    MyEchoClient client(&loop, InetAddress("127.0.0.1", 9000));
    client.connect();
    loop.loop();
    return 0;
}
