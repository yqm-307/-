#pragma once
#include "util.h"
#include "acceptor.h"

/**
 * @brief sms客户端
 *  实现json解析、保存所有json数据，转换为get url资源，发送到腾讯邮箱域
 */
class SMSClient
{
public:
    typedef std::vector<std::string> JsonList;

    SMSClient(boost::asio::io_context&);
    ~SMSClient();
    
    //接收json数据
    int pushAJson(std::string& json);



private:
    void send();
    //制作成Get请求url
    int makeGetUrl(std::string& GetRes);
private:
    JsonList datas_;
    boost::mutex lock_;
    CURL* curl_;

    std::string key_;
    std::string my_;
};
