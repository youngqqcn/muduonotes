#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <unordered_set>
#include <boost/circular_buffer.hpp>

using namespace muduo;
using namespace muduo::net;

class EchoServer : noncopyable
{
public:
    EchoServer(EventLoop *pLoop, const InetAddress &addr, string name, int nIdleSeconds)
        : _m_pLoop(pLoop),
          _m_server(_m_pLoop, addr, name),
        _m_connBuckets(nIdleSeconds)
    {
        _m_server.setConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
        _m_server.setMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2, _3));
        _m_pLoop->runEvery(2, std::bind(&EchoServer::onTimer, this));
        _m_connBuckets.resize(nIdleSeconds);
    }

    void start()
    {
        _m_server.start();
    }

private:
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp tm)
    {
        LOG_INFO << "=============onMessage=============";
        string msg(buf->retrieveAllAsString());
        LOG_INFO << "msg is is " << msg;
        conn->send(msg); //消息返回

        assert(!conn->getContext().empty());
        WeakEntryPtr weakEntry( boost::any_cast<WeakEntryPtr>(conn->getContext()) );
        EntryPtr entry( weakEntry.lock() );
        if(entry)
        {
            _m_connBuckets.back().insert(entry); // 获取最后一个Bucket(unsorted_set)并插入元素
            dumpConnectionBuckets();
        }

    }

    void onConnection(const TcpConnectionPtr &conn)
    {
        LOG_INFO << "=============onConnection=============";
        if (conn->connected())
        {
            EntryPtr entry(new Entry(conn));
            _m_connBuckets.back().insert(entry);
            dumpConnectionBuckets();
            WeakEntryPtr weakEntry(entry);
            conn->setContext(weakEntry);
        }
        else
        {
            assert(!conn->getContext().empty());
            WeakEntryPtr weakEntry( boost::any_cast<WeakEntryPtr>(conn->getContext()) );
            LOG_INFO << "Entry use count = " << weakEntry.use_count();
        }
    }


    void dumpConnectionBuckets() const
    {
        LOG_INFO << "size = " << _m_connBuckets.size();
        int idx = 0;
        for (WeakConnectionList::const_iterator bucketI = _m_connBuckets.begin();
             bucketI != _m_connBuckets.end();
             ++bucketI, ++idx)
        {
            const Bucket &bucket = *bucketI;
            printf("[%d] len = %zd : ", idx, bucket.size());
            for (const auto &it : bucket)
            {
                bool connectionDead = it->m_weakConn.expired();
                printf("%p(%ld)%s, ", get_pointer(it), it.use_count(),
                       connectionDead ? " DEAD" : "");
            }
            puts("");
        }
    }

    typedef std::weak_ptr<TcpConnection> WeakTcpConnectionPtr;
    struct Entry : public muduo::copyable
    {
        explicit Entry(const WeakTcpConnectionPtr &weakConn)
            : m_weakConn(weakConn)
        {
        }

        ~Entry()
        {
            auto conn = m_weakConn.lock();
            if (conn)
            {
                LOG_INFO << "connection idle time timeout, close it";
                // shutdown关闭了server写入端,这时client会read返回0; 
                // 此时, client仍然是可以写入的. 因为正确情况下, 
                // client在read返回0时知道了服务端关闭了写端,在处理之后剩下的数据之后应该主动关闭连接
                conn->shutdown(); 
            }
        }
        WeakTcpConnectionPtr m_weakConn;
    };

    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<Entry> WeakEntryPtr;
    typedef std::unordered_set<EntryPtr> Bucket;
    typedef boost::circular_buffer<Bucket> WeakConnectionList;

    void onTimer()
    {
        LOG_INFO << "============onTimer()";
        _m_connBuckets.push_back(Bucket());
        dumpConnectionBuckets();
    }

    

private:
    EventLoop *_m_pLoop;
    TcpServer _m_server;

    WeakConnectionList _m_connBuckets;
};
