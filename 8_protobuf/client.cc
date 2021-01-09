#include <stdio.h>
#include <muduo/net/TcpClient.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include "query.pb.h"
#include "dispatcher.h"
#include "codec.h"

#include <functional>

using namespace muduo;
using namespace muduo::net;
using namespace yqq;

using namespace std::placeholders; // for _4

typedef std::shared_ptr<yqq::Query> QueryPtr;
typedef std::shared_ptr<yqq::Answer> AnswerPtr;

class Client : noncopyable
{
public:
    Client(EventLoop *pLoop, const InetAddress &addr, string name)
        : _m_pLoop(pLoop),
          _m_client(pLoop, addr, name),
          _m_dispatcher(std::bind(&Client::defaultMessageCb, this, _1, _2, _3)),
          _m_codec(std::bind(&ProtobufDispatcher::onProtobufMessage, &_m_dispatcher, _1, _2, _3))
    {

        // _m_dispatcher.registerMessageCallback<yqq::Query>(std::bind(&Client::onQuery, this, _1, _2, _3));
        _m_dispatcher.registerMessageCallback<yqq::Answer>(std::bind(&Client::onAnswer, this, _1, _2, _3));
        _m_client.setMessageCallback(std::bind(&ProtobufCodec::onMessage, &_m_codec, _1, _2, _3));
        _m_client.setConnectionCallback(std::bind(&Client::onConnection, this, _1));
    }

    void connect()
    {
        _m_client.connect();
    }

private:
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            LOG_INFO << "==========> onConnection";
            Query query;
            query.set_id(1);
            query.set_questioner("client");
            //  query.set_question(0, "1+1=?");
            //  query.set_question(0, "2+2=?");
            query.add_question("1+1=?");
            query.add_question("2+2=?");
            query.add_question("3+3=?");

            _m_codec.send(conn, query);
        }
        else
        {
            LOG_INFO << "server closed connection";
        }
    }

    void defaultMessageCb(const TcpConnectionPtr &conn, const MessagePtr &message, Timestamp tm)
    {

        LOG_INFO << message->GetTypeName() << "not support";
    }

    // void onQuery(const muduo::net::TcpConnectionPtr &conn,
    //              const QueryPtr &message,
    //              muduo::Timestamp)
    // {
    //     // LOG_INFO << "onQuery" <<
    // }

    void onAnswer(const muduo::net::TcpConnectionPtr &conn,
                  const AnswerPtr &message,
                  muduo::Timestamp)
    {
        LOG_INFO << "onAnswer:  " << message->DebugString();
    }

    EventLoop *_m_pLoop;
    TcpClient _m_client;
    ProtobufDispatcher _m_dispatcher;
    ProtobufCodec _m_codec;
};

int main(int argc, char const *argv[])
{
    EventLoop loop;
    Client client(&loop, InetAddress(9000), "client");
    client.connect();
    loop.loop();
    printf("client\n");
    return 0;
}
