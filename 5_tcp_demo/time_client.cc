#include <stdio.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>
#include <muduo/base/Logging.h>
#include <muduo/base/noncopyable.h>
#include <functional>

using namespace muduo;
using namespace muduo::net;

class TimeClient : noncopyable
{
public:
    TimeClient(EventLoop *loop, const InetAddress &serverAddr)
        : _m_loop(loop),
          _m_client(_m_loop, serverAddr, "TimeClient")
    {
        _m_client.setConnectionCallback(std::bind(&TimeClient::onConnection, this, _1));
        _m_client.setMessageCallback(std::bind(&TimeClient::onMessage, this, _1, _2, _3));
    }

    void connect()
    {
        _m_client.connect();
    }

private:
    EventLoop *_m_loop;
    TcpClient _m_client;

    void onConnection(const TcpConnectionPtr &conn)
    {
        LOG_INFO << conn->peerAddress().toIpPort() << (conn->connected() ? "UP" : "DOWN");
    }

    void onMessage(const TcpConnectionPtr &conn, Buffer *pBuf, Timestamp timestamp)
    {
        if (pBuf->readableBytes() >= sizeof(int32_t))
        {
            int32_t tm32 = pBuf->peekInt32();
            pBuf->retrieve(sizeof(tm32));
            Timestamp ts(tm32 * Timestamp::kMicroSecondsPerSecond);
            LOG_INFO << " server return time: " << tm32 << "  , " << ts.toFormattedString();
        }
        else
        {
            // 不够四个字节
            LOG_INFO << conn->name() << " data length is not enough for time_t, length:" << pBuf->readableBytes() << timestamp.toFormattedString();
        }
    }
};

int main(int argc, char const *argv[])
{
    if(argc < 1)
    {
        printf("Usage: %s host_ip\n", argv[0]);
        return 0;
    }

    EventLoop loop;
    TimeClient  timeclient(&loop, InetAddress(argv[1], 9000));
    timeclient.connect();
    loop.loop();

    return 0;
}
