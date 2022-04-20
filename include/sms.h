#pragma once
#include "util.h"


struct Req
{
    std::string senduser;   //发送者邮箱
    std::string recvuser;   //接收者邮箱
    std::string sqm;        //qq邮箱授权码
    std::string Data;       //数据内容
};

/**
 * @brief sms客户端
 *  实现json解析、保存所有json数据，转换为get url资源，发送到腾讯邮箱域
 */
class SMSClient
{
public:
    typedef std::queue<Req> MessageQueue;

    SMSClient(boost::asio::io_context&);
    ~SMSClient();
    
    //接收json数据
    void pushAJson(std::string& json);
    
private:
    Req json_handle(std::string& GetRes);   //解析json
    void close();

private:
    std::string base64(std::string&);   //base64加密
    void WorkThread_main();
    bool workThread_login(Req&);    //登录
    bool workThread_relogin(Req&);  //重新登录
    //void workThread_loginoff(); //退出登录
    bool workThread_sendData(Req&);
    std::string workThread_sendline(std::string&); //发送一行
private:
    boost::asio::io_context& ioc_;
    MessageQueue mq_;
    boost::mutex lock_;
    boost::asio::ip::tcp::socket sockClient_;
    boost::atomic_bool Connecting_;

};
/*
MAIL FROM: <979336542@qq.com>
RCPT TO:<373232355@qq.com>
DATA
SUBJECT:nihao
你好
.

*/