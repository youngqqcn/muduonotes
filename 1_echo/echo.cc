#include "echo.h"
#include <functional>
#include <muduo/base/Logging.h>

EchoServer::EchoServer(EventLoop* loop, InetAddress& listenAddr)
    :_m_pLoop(loop), _m_tcpServer(loop, listenAddr, "EchoServer")
{
    _m_tcpServer.setConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
    _m_tcpServer.setMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2, _3));
}

void EchoServer::start() 
{
    _m_tcpServer.start();
}


void EchoServer::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO << "Echoserver connect from " << conn->peerAddress().toIpPort()
    << " -> " << conn->localAddress().toIpPort() << " is " 
    << (conn->connected() ? ("UP") : ("DOWN") );
}

void EchoServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
{
    std::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << "  echo  " << msg.size() << "  bytes,  " 
    << " data received at " << time.toString();

    conn->send(msg);
}
