#pragma once

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "TcpConnection.h"
#include "util.h"


class Acceptor: boost::noncopyable
{
public:
    typedef boost::function<void(TcpConnection::ConnectionPtr,std::string&)> OnConnectCallback;
    typedef boost::function<std::string(std::string&&)> OnRecvCallback;
    Acceptor(boost::asio::io_context& ioc,const tcp::endpoint& edp)
        :ioc_(ioc),
        acceptor_(ioc,edp)
    {
        DEBUG("acceptor init success!");
    }

    void listen()
    {

        boost::asio::steady_timer timer(ioc_,boost::asio::chrono::seconds(5));    //超时5s
        boost::system::error_code err;
        boost::function<void(const boost::system::error_code&,boost::asio::steady_timer*)> timer_handle=
        [&timer_handle](const boost::system::error_code& err,boost::asio::steady_timer* timer){
            if(err)
                ERROR("listen::boost::asio::steady_timer error: %s",err.message().c_str());
            INFO("listen time out");
            timer->cancel();
            timer->async_wait(boost::bind(timer_handle,err,timer));
        };

        timer.async_wait(boost::bind(timer_handle,err,&timer));

        while(true)
        {
            TcpConnection::ConnectionPtr conn = TcpConnection::create(ioc_);
            /*listen*/
            conn->socket() = acceptor_.accept();
            char recvbuf[1024];
            memset(recvbuf,'\0',sizeof(recvbuf));
            conn->socket().receive(boost::asio::buffer(recvbuf));
            std::string res = onrecvcb_(std::string(recvbuf));
            INFO("recv\n%s",recvbuf);
            onconnectcb_(conn,res);
        }
    }

    void setOnConnectionCallBack(OnConnectCallback&& cb)
    { onconnectcb_ = cb; } 
    void setOnRecvCallback(OnRecvCallback&& cb)
    { onrecvcb_ = cb; }
private:
    boost::asio::io_context& ioc_;
    tcp::acceptor acceptor_;
    OnConnectCallback onconnectcb_;
    OnRecvCallback onrecvcb_;
};