#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "TcpConnection.h"
#include "util.h"


class Acceptor: boost::noncopyable
{
public:
    typedef boost::function<void(TcpConnection::ConnectionPtr,const boost::system::error_code&)> OnConnectCallback;

    Acceptor(boost::asio::io_context& ioc,const tcp::endpoint& edp)
        :ioc_(ioc),
        acceptor_(ioc,edp)
    {}

    void listen()
    {
        while(true)
        {
            TcpConnection::ConnectionPtr conn = TcpConnection::create(ioc_);
            /*listen*/
            conn->socket() = acceptor_.accept();
            
                while(true)
                {    
                    char recvbuf[1024];
                    memset(recvbuf,'\0',sizeof(recvbuf));
                    conn->socket().receive(boost::asio::buffer(recvbuf));
                    INFO("recv\n%s",recvbuf);
                }
            
        }
    }

    void setOnConnectionCallBack(OnConnectCallback&& cb)
    { onconnectcb_ = cb; } 

private:
    boost::asio::io_context& ioc_;
    tcp::acceptor acceptor_;
    OnConnectCallback onconnectcb_;
};