#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "util.h"


class Acceptor: boost::noncopyable
{
public:
    Acceptor(boost::asio::io_context& ioc,const tcp::endpoint& edp)
        :socket_(ioc),
        acceptor_(ioc,edp),
        serveraddr_(edp)
    {}

    void listen()
    {
        while(true)
        {
            /*listen*/
            acceptor_.async_accept();
        }
    }

    void setOnConnectionCallBack(OnConnectCallback& cb)
    { onconnectcb_ = cb; } 

    

private:
    tcp::socket socket_;
    boost::asio::ip::tcp::acceptor acceptor_;
    tcp::endpoint serveraddr_;
    OnConnectCallback onconnectcb_;
};