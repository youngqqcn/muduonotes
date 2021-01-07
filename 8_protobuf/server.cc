#include <stdio.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

#if 0
// #include <muduo/net/protobuf/ProtobufCodecLite.h>
#else
#include "codec.h"
#endif

#include "query.pb.h"
#include "dispatcher.h"

#include <functional>

using namespace muduo;
using namespace muduo::net;
using namespace yqq;

using namespace std::placeholders; // for _4

typedef std::shared_ptr<yqq::Query> QueryPtr;
typedef std::shared_ptr<yqq::Answer> AnswerPtr;

class Server : noncopyable
{
public:
    Server(EventLoop *loop, const InetAddress &addr, string name) 
    : _m_pLoop(loop), _m_server(_m_pLoop, addr, name),
      _m_dispatcher(std::bind(&Server::defaultMessageCb, this, _1, _2, _3)),
      _m_codec(std::bind(&ProtobufDispatcher::onProtobufMessage, &_m_dispatcher, _1, _2, _3),
      std::bind(&Server::onPbErrMssage, this, _1, _2, _3, _4))
    {

        _m_dispatcher.registerMessageCallback<yqq::Query>(std::bind(&Server::onQuery, this, _1, _2, _3));
        _m_dispatcher.registerMessageCallback<yqq::Answer>(std::bind(&Server::onAnswer, this, _1, _2, _3));
        _m_server.setConnectionCallback(std::bind(&Server::onConnection, this, _1));
        _m_server.setMessageCallback(std::bind(&ProtobufCodec::onMessage, &_m_codec, _1, _2, _3));
    }

    void start()
    {
        _m_server.start();
    }

private:
    // void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp tm)
    // {
    //     LOG_INFO << "onMessage() ================";
    // }

    void onQuery(const muduo::net::TcpConnectionPtr &conn,
                 const QueryPtr &message,
                 muduo::Timestamp)
    {
        LOG_INFO << "=============onQuery:" << message->GetTypeName() 
        << " questioner  " << message->DebugString();

        Answer ans;
        ans.set_id(666);
        ans.set_questioner(message->questioner());
        ans.set_answerer("yqq");
        ans.add_solution("solution1");
        ans.add_solution("solution2");
        ans.add_solution("solution3");
        ans.add_solution("solution4");
    }

    void onAnswer(const muduo::net::TcpConnectionPtr &conn,
                  const AnswerPtr &message,
                  muduo::Timestamp)
    {
        LOG_INFO << "=============onAnswer";
    }

    void onPbErrMssage(const muduo::net::TcpConnectionPtr &conn,
                       muduo::net::Buffer *buf,
                       muduo::Timestamp tm,
                       ProtobufCodec::ErrorCode code)
    {
        LOG_INFO << "+++++++=====ERROR";
    }

    void defaultMessageCb(const TcpConnectionPtr &conn, const MessagePtr &message, Timestamp tm)
    {

        LOG_INFO << message->GetTypeName() << "not support";
    }

    void onConnection(const TcpConnectionPtr &conn)
    {
        LOG_INFO << "onConnection() ================";
    }

    EventLoop *_m_pLoop;
    TcpServer _m_server;

    ProtobufDispatcher _m_dispatcher;

    // ProtobufCodecLiteT<> _m_codec;
    ProtobufCodec _m_codec;
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
