#include "../include/sms.h"

using namespace boost::chrono;

SMSClient::SMSClient(boost::asio::io_context& ioc)
    :sockClient_(ioc),
    ioc_(ioc),
    Connecting_(false)
{
    sockClient_.non_blocking(true); //设置套接字非阻塞
}

SMSClient::~SMSClient()
{
}

bool SMSClient::workThread_login(Req& conn)    //登录流程
{

    std::string msg = "auth login\n\t";
    std::string s = workThread_sendline(msg);
    if(!is_in_str(s,"334")){
        FATAL("can`t login smtp.qq.com");
        return false;
    }

    msg = base64(conn.senduser);
    s = workThread_sendline(msg);   //账号
    if(!is_in_str(s,"334")){
        FATAL("can`t login smtp.qq.com");
        return false;
    }

    msg = base64(conn.sqm);
    s = workThread_sendline(msg);   //授权码
    if(!is_in_str(s,"235")){
        FATAL("can`t login smtp.qq.com");
        return false;
    }

    msg = "ehlo hello\n\t";
    s = workThread_sendline(msg);   
    
    return true;
}

bool SMSClient::workThread_sendData(Req& conn)
{
    std::string msg="";
    std::string s="";
    //发送者
    msg = "email from:<"+conn.senduser+">\n\t";
    s = workThread_sendline(msg);   
    if(!is_in_str(s,"250")){
        FATAL("Format error");
        return false;
    }

    //接收者
    msg = "rcpt to:<"+conn.recvuser+">\r\n";
    s = workThread_sendline(msg);
    if(!is_in_str(s,"250")){
        FATAL("Format error");
        return false;
    }

    //提示开始传输数据
    msg = "DATA\r\n";
    s = workThread_sendline(msg);
    if(!is_in_str(s,"354")){
        FATAL("Format error");
        return false;
    }

    //传输正文
    msg = conn.Data+"\r\n.\r\n";
    s = workThread_sendline(msg);
    if(!is_in_str(s,"250")){
        FATAL("Format error");
        return false;
    }

    return true;
}

bool SMSClient::workThread_relogin(Req&conn)  //重新登录
{
    return workThread_login(conn);
}

inline bool is_in_str(std::string& main,std::string sub)
{
    return (main.find(sub)!=std::string::npos);
}

std::string SMSClient::workThread_sendline(std::string&line) //发送一行
{
    std::string ret="";
    int nbyte_to_send = 0;
    while(nbyte_to_send != line.size())
    {
        nbyte_to_send+=sockClient_.send(boost::asio::buffer(std::string(line.begin()+nbyte_to_send,line.end())));
        INFO("[send] bytes_num: %d",nbyte_to_send);
    }
    INFO("[send] send once over!\n\t%s",line.c_str());
    
    //接收res
    int n = sockClient_.receive(boost::asio::buffer(ret));
    if(n = EOF)
        INFO("对端关闭");
    INFO("[recv] The terminal receives %d bits",n);
    return ret;
}

void SMSClient::WorkThread_main()
{
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("157.148.54.34"),25);
    sockClient_.async_connect(ep,std::bind([this](const boost::system::error_code& err){
        if(err)
        {
            ERROR("%s",err.message().c_str());
        }      
        Connecting_.exchange(true);
    },boost::asio::placeholders::error));  //连接上smtp.qq.com
     
    while(Connecting_);   //等待到连接成功

    boost::atomic_bool timeout(false);

    int t = 60;

    //直到超时60s没有消息退出
    while(true)
    {
        boost::asio::steady_timer timer(ioc_,boost::asio::chrono::seconds(t)); 
        

        if(!mq_.empty())
        {
            //如果有消息，处理并重置超时时间
            timer.cancel();
            timer.async_wait(boost::bind([&timeout](const boost::system::error_code& err,boost::asio::steady_timer& t){
                if(err)
                    ERROR("%s",err.message().c_str());
                timeout.exchange(true);//通知超时
            },boost::asio::placeholders::error));

            Req once;
            {
                boost::lock_guard<boost::mutex> lock(lock_);
                once = mq_.front();
                mq_.pop();  //出队
            }
            //执行
            if(workThread_login(once))
                INFO("邮件发送成功");
            else
                ERROR("邮件发送失败");
        }//mq空

        if(timeout)
            INFO("timeout:%d workthread over",t);
    }


    //即将退出，关闭连接
    std::string s = "quit\n\t";
    workThread_sendline(s);
    close();
}
void SMSClient::close()
{
    sockClient_.close();
}

void SMSClient::pushAJson(std::string& json)
{
    boost::lock_guard<boost::mutex> lock(lock_);
    mq_.emplace(json_handle(json));

}


/*
MAIL FROM: <979336542@qq.com>
RCPT TO:<373232355@qq.com>
DATA
SUBJECT:nihao
你好
.
*/
//json解析为Req
Req SMSClient::json_handle(std::string& GetRes)
{
    Json::Reader reader;
    Json::Value root;

    int ret = reader.parse(GetRes,root);

    Req tmp;
    tmp.senduser = root["senduser"].asString();
    tmp.recvuser = root["recvuser"].asString();
    tmp.sqm = root["sqm"].asString();
    tmp.Data = root["data"].asString();

    return tmp;

}

using namespace std;
// base64 加密函数
string base64(string str)   //base64加密算法
{
    string base64_table="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int str_len = str.length();
    string res="";
    for (int strp=0; strp<str_len/3*3; strp+=3)
    {
        res+=base64_table[str[strp]>>2];
        res+=base64_table[(str[strp]&0x3)<<4 | (str[strp+1])>>4];
        res+=base64_table[(str[strp+1]&0xf)<<2 | (str[strp+2])>>6];
        res+=base64_table[(str[strp+2])&0x3f];
        //cout<<res<<endl;
    }
    if (str_len%3==1)
    {
        int pos=str_len/3 * 3;
        res += base64_table[str[pos]>>2];
        res += base64_table[(str[pos]&0x3)<<4];
        res += "=";
        res += "=";
    }
    else if (str_len%3==2)
    {
        int pos=str_len/3 * 3;
        res += base64_table[str[pos]>>2];
        res += base64_table[(str[pos]&0x3)<<4 | (str[pos+1])>>4];
        res += base64_table[(str[pos+1]&0xf)<<2];
        res += "=";
    }
    return res;
}