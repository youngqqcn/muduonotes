#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/ThreadPool.h>

#include "sudoku.h"
#include <algorithm>
#include <functional>

using namespace muduo;
using namespace muduo::net;


class SudokuServer
{
public:
    SudokuServer(int nThreadNums, EventLoop *loop, const InetAddress &listenAddr, string name)
        : _m_nThreadNums(nThreadNums), _m_server(loop, listenAddr, name)
    {
        _m_server.setConnectionCallback(std::bind(&SudokuServer::onConnection, this, _1));
        _m_server.setMessageCallback(std::bind(&SudokuServer::onMessage, this, _1, _2, _3));
    }

    void start()
    {
        _m_server.start();
        _m_threadPool.start(_m_nThreadNums);
    }

    void onConnection(const TcpConnectionPtr &conn)
    {
        LOG_INFO << conn->peerAddress().toIpPort() << "  " << conn->localAddress().toIpPort() 
            << " is " << ((conn->connected()) ? (" UP ") : (" DOWN "));
    }

    bool processRequest(const TcpConnectionPtr &conn, const string &request)
    {
        string id;
        string puzzle;
        bool goodRequest = true;

        string::const_iterator colon = std::find(request.begin(), request.end(), ':');
        if (colon != request.end())
        {
            id.assign(request.begin(), colon);
            puzzle.assign(colon + 1, request.end());
        }
        else
        {
            puzzle = request;
        }

        if (puzzle.size() == implicit_cast<size_t>(kCells))
        {
            _m_threadPool.run([conn, id, puzzle]() { // 需要使用值传递, 不能使用引用传递
                LOG_INFO << "puzzle is " << puzzle;
                LOG_INFO << conn->name() << "thread_id: " << gettid();
                string result = solveSudoku(puzzle);
                if (id.empty())
                {
                    conn->send(result + "\r\n");
                }
                else
                {
                    conn->send(id + ":" + result + "\r\n");
                }
            });
        }
        else
        {
            goodRequest = false;
        }
        return goodRequest;
    }

    void onMessage(const TcpConnectionPtr &conn, Buffer *pBuf, Timestamp timestamp)
    {
        LOG_DEBUG << conn->name();
        size_t len = pBuf->readableBytes();
        while (len > kCells + 2)
        {
            const char *crlf = pBuf->findCRLF();
            if (crlf)
            {
                string reqData(pBuf->peek(), crlf);
                pBuf->retrieveUntil(crlf + 2);
                len = pBuf->readableBytes();
                if (!processRequest(conn, reqData))
                {
                    conn->send("bad request");
                    conn->shutdown();
                    break;
                }
            }
            else if (len > 100)
            {
                conn->send("data is too long");
                conn->shutdown();
                break;
            }
            else
            {
                break;
            }
        }
    }

private:
    TcpServer _m_server;
    ThreadPool _m_threadPool;
    int _m_nThreadNums;
};

int main(int argc, char *argv[])
{
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    int numThreads = 2;
    if (argc > 1)
    {
        numThreads = atoi(argv[1]);
    }
    else
    {
        numThreads = 4;
    }
    

    EventLoop loop;
    SudokuServer server(numThreads, &loop, InetAddress(9000), "sudoku");
    server.start();
    loop.loop();

    return 0;
}