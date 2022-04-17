#include "util.h"


class TcpConnection: boost::enable_shared_from_this<TcpConnection>
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
    static ConnectionPtr create(boost::asio::io_context& ioc)
    {
        return ConnectionPtr(new TcpConnection(ioc));   //智能指针传出，由当前类控制资源
    }

    void async_write(std::string msg)
    {
        
    }

private:
    tcp::socket client_;
    boost::asio::io_context& ioc_;
};