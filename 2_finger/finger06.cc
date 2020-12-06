#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <muduo/net/TcpServer.h>
#include <map>
#include <utility>


using namespace muduo;
using namespace muduo::net;

typedef std::map<string, string> UserMap;

UserMap g_Users;

string getUser(const string& strUserName)
{
    string strResult;
    auto it = g_Users.find(strUserName);
    if(g_Users.end() != it)
    {
        strResult =  it->second;
    }
    return strResult;
}

void onMessage(const TcpConnectionPtr& conn, Buffer* pBuf, Timestamp timestamp)
{
    const char* crlf = pBuf->findCRLF();
    if(crlf)
    {
        string user(pBuf->peek(), crlf);
        conn->send(getUser(user) + "\r\n");
        pBuf->retrieveUntil(crlf + 2);
        conn->shutdown();
    }
}

int main()
{
    g_Users.insert(std::make_pair("yqq", "goooooood"));
    EventLoop loop;
    TcpServer server(&loop, InetAddress(9000), "finger06");
    server.setMessageCallback(onMessage);
    server.start();
    loop.loop();
    
    return 0;
}