#pragma once
#include "util.h"
#include "acceptor.h"

class Server
{
public:
    typedef std::vector<TcpConnection::ConnectionPtr> ConnectionList;
    Server(boost::asio::io_context& ioc,tcp::endpoint&& edp)
        :ioc_(ioc),
        acceptor_(ioc,edp),
        users_()
    {
        acceptor_.setOnConnectionCallBack(
                [this]( TcpConnection::ConnectionPtr newconn)
                {
                    users_.push_back(newconn);
                    //todo 解析json，发送邮件
                    INFO("a new connection form %s",newconn->socket().local_endpoint().address().to_string().c_str());
                }
            );
    }


    //服务器开始运行
    void start()
    {
        acceptor_.listen();
    }

private:
    void handle();
private:
    boost::asio::io_context& ioc_;
    Acceptor acceptor_;
    ConnectionList users_;
};