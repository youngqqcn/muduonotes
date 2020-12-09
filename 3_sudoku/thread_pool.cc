#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/ThreadPool.h>

#include "sudoku.h"
#include <algorithm>

using namespace muduo;
using namespace muduo::net;

ThreadPool threadPool;

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
        threadPool.run([conn, id, puzzle]() { // 需要使用值传递, 不能使用引用传递
            LOG_INFO << "puzzle is " << puzzle;
            LOG_INFO<< conn->name() << "thread_id: " << gettid();
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

int main(int argc, char *argv[])
{
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    int numThreads = 0;
    if (argc > 1)
    {
        numThreads = atoi(argv[1]);
    }

    EventLoop loop;
    TcpServer server(&loop, InetAddress(9000), "sudoku");
    server.setMessageCallback(onMessage);
    threadPool.start(numThreads);
    server.start();

    loop.loop();

    return 0;
}