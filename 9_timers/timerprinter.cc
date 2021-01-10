#include <stdio.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/base/TimeZone.h>

using namespace muduo;
using namespace muduo::net;


class Printer : noncopyable 
{
public:
    Printer(EventLoop* pLoop)
    : _m_pLoop(pLoop)
    {
        _m_pLoop->runAfter(1, std::bind(&Printer::printAfter, this));
        _m_pLoop->runEvery(1, std::bind(&Printer::printEvery, this));



        auto now = Timestamp::now();
        auto after = Timestamp::fromUnixTime(now.secondsSinceEpoch() + 10); // 10s后执行一次
        _m_pLoop->runAt( after, std::bind(&Printer::printAt, this) );
    }

    void printAfter()
    {
        LOG_INFO << "==============> printAfter";
        // _m_pLoop->quit();
    }

    void printEvery()
    {
        LOG_INFO << "==============> printEvery";
    }

    void printAt()
    {
        LOG_INFO << "=====> printAt";
    }

private:
    EventLoop *_m_pLoop;
}; 


int main(int argc, char const *argv[])
{
    // printf("hello\n");
    EventLoop loop;
    Printer printer(&loop);
    loop.loop();
    return 0;
}
