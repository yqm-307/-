#pragma once
#include "util.h"
#include "acceptor.h"
#include "sms.h"


class Server
{
public:
    typedef std::vector<TcpConnection::ConnectionPtr> ConnectionList;
    Server(boost::asio::io_context& ioc,tcp::endpoint&& edp)
        :ioc_(ioc),
        acceptor_(ioc,edp),
        //users_(),
        sms_(ioc)
    {
        acceptor_.setOnConnectionCallBack(
                [this]( TcpConnection::ConnectionPtr newconn)
                {
                    INFO("a new connection form %s",newconn->socket().local_endpoint().address().to_string().c_str());
                    std::string res = "200 ok connect";
                    newconn->socket().async_send(boost::asio::buffer(res),
                    [newconn](const boost::system::error_code& err,size_t bytes_transferred){
                            if(err)
                                ERROR("%s",err.message().c_str());
                            else
                                INFO("send res --> %s",newconn->socket().remote_endpoint().address().to_string().c_str());
                    });
                }
        );

        acceptor_.setOnRecvCallback([this](const std::string& recv){
                sms_.pushAJson(const_cast<std::string&>(recv));
        });
        DEBUG("server init success!");
    }


    //服务器开始运行
    void start()
    {
        DEBUG("server start!");
        acceptor_.listen();
    }

private:
    void handle();
private:
    boost::asio::io_context& ioc_;
    Acceptor acceptor_;
    //ConnectionList users_;
    SMSClient sms_;
};