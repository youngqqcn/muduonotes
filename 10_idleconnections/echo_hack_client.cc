#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>
#include <muduo/base/Logging.h>
#include <muduo/base/noncopyable.h>
#include <muduo/base/Thread.h>

#include <functional>

using namespace muduo;
using namespace muduo::net;

class EchoHackClient : noncopyable
{
public:
    EchoHackClient(EventLoop *ploop, const InetAddress &address)
        : _m_pLoop(ploop),
          _m_client(ploop, address, "EchoHackClient")
    {
        _m_client.setConnectionCallback(std::bind(&EchoHackClient::onConnection, this, _1));
        _m_client.setMessageCallback(std::bind(&EchoHackClient::onMessage, this, _1, _2, _3));
    }

    void connect()
    {
        _m_client.connect();
        LOG_INFO << _m_client.name() << " connected succeed ";
    }

private:
    EventLoop *_m_pLoop;
    TcpClient _m_client;

    void onMessage(const TcpConnectionPtr &conn, Buffer *pBuf, Timestamp timestamp)
    {
        LOG_INFO << "length: " << pBuf->readableBytes();

        // 不读取缓冲区的数据, 服务端的发送缓冲区会堆积数据, 导致内存逐渐增加
        // pBuf->retrieveAll();
    }

    void onConnection(const TcpConnectionPtr &conn)
    {
        LOG_INFO << "=========>onConnection";
        // char buf[1024 * 4] = {1};
        // while(true)
        // {
        //     conn->send(buf, sizeof(buf));

        //     //避免发送太快 导致内存占用太多被操作系统杀掉
        //     CurrentThread::sleepUsec(1000); 
        // }
        // LOG_INFO << "connected" << conn->peerAddress().toIpPort();
    }
};

int main(int argc, char *argv[])
{
    // if (argc < 2)
    // {
    //     printf("%s host_ip\n", argv[0]);
    //     return 0;
    // }

    EventLoop loop;
    // InetAddress serverAddress(argv[1], 9000);
    // EchoHackClient EchoHackClient(&loop, serverAddress);
    EchoHackClient EchoHackClient(&loop, InetAddress(9000));
    EchoHackClient.connect();
    loop.loop();
    return 0;
}