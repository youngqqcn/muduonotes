#include <stdio.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/noncopyable.h>

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
    // EventLoop *pLoop;

    void onMessage(const TcpConnectionPtr &conn, Buffer *pBuf, Timestamp timestamp)
    {
    }

    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            LOG_INFO << "client connected";
            string content = readFile("./test.txt");
            conn->send(content);
            conn->shutdown();
        }
        else
        {
            LOG_INFO << " client closed  connection ";
        }
    }
};

int main(int argc, char const *argv[])
{
    /* code */
    // printf("hello\n");

    // string content = FileTransfer::readFile("./test.txt");
    // LOG_INFO <<  "content.size = " << content.size() << "content is" << content;

    EventLoop loop;
    FileTransfer filestransfer(&loop, InetAddress(9000));
    filestransfer.start();
    loop.loop();

    return 0;
}
