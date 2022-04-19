#pragma once
#include "util.h"


class TcpConnection
{
public:
    typedef boost::shared_ptr<TcpConnection> ConnectionPtr;

    TcpConnection(boost::asio::io_context& ioc)
        :client_(ioc),
        ioc_(ioc)
    {}
    ~TcpConnection()
    {
        client_.close();
        /*log*/
    }

    tcp::socket& socket()
    { return client_; }

    static ConnectionPtr create(boost::asio::io_context& ioc)
    {
        return ConnectionPtr(new TcpConnection(ioc));   //智能指针传出，由当前类控制资源
    }



private:


private:
    tcp::socket client_;
    boost::asio::io_context& ioc_;
};