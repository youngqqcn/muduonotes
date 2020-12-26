#include <stdio.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/noncopyable.h>
// #include <boost/any.hpp>

using namespace muduo;
using namespace muduo::net;

class FileTransfer : noncopyable
{
public:
    FileTransfer(EventLoop *loop, const InetAddress &address)
        : _m_server(loop, address, "FileTransfer")
    {
        _m_server.setConnectionCallback(std::bind(&FileTransfer::onConnection, this, _1));
        _m_server.setMessageCallback(std::bind(&FileTransfer::onMessage, this, _1, _2, _3));
        _m_server.setWriteCompleteCallback(std::bind(&FileTransfer::onWriteComplete, this, _1));
    }

    static string readFile(const char *filename)
    {
        string ret;
        FILE *fp = fopen(filename, "rb");
        if (NULL == fp)
        {
            printf("not found  error\n");
            return ret;
        }
        while (1)
        {
            char buf[1024] = {0};
            size_t nret = fread(buf, 1, sizeof(buf), fp);
            if (nret <= 0)
            {
                // 读完了
                fclose(fp);
                fp = NULL;
                break;
            }
            else
            {
                ret.append(buf, nret);
            }
        }
        return ret;
    }

    void start()
    {
        _m_server.start();
    }

private:
    TcpServer _m_server;

    void onMessage(const TcpConnectionPtr &conn, Buffer *pBuf, Timestamp timestamp)
    {
    }

    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            LOG_INFO << "client connected";

            FILE *fp = ::fopen("./test.txt", "rb");
            if (fp)
            {
                conn->setContext(fp);
                char buf[1024];
                size_t nRead = ::fread(buf, 1, sizeof(buf), fp);
                if (nRead > 0)
                {
                    // LOG_INFO << "===============";
                    // conn->send(buf, nRead);
                    // LOG_INFO << "===============";
                    conn->send(buf, nRead);
                }
            }
            else
            {

                conn->shutdown();
                LOG_ERROR << "open file error";
            }
        }
        else
        {
            if (!conn->getContext().empty())
            {
                FILE *fp = boost::any_cast<FILE *>(conn->getContext());
                if (fp)
                {
                    ::fclose(fp);
                }
            }
            LOG_INFO << " client closed  connection ";
        }
    }

    // conn->send 发送完毕之后会回调此方法
    void onWriteComplete(const TcpConnectionPtr &conn)
    {
        LOG_INFO << "onWriteComplete";

        FILE *fp = boost::any_cast<FILE *>(conn->getContext());
        if (fp)
        {
            char buf[1024];
            size_t nread = ::fread(buf, 1, sizeof(buf), fp);
            if (nread)
            {
                conn->send(buf, nread);
            }
            else
            {
                ::fclose(fp);
                fp = NULL;
                conn->setContext(fp);
                conn->shutdown();
                LOG_INFO << "send finished";
            }
        }
    }
};

int main(int argc, char const *argv[])
{

    EventLoop loop;
    FileTransfer filestransfer(&loop, InetAddress(9000));
    filestransfer.start();
    loop.loop();

    return 0;
}