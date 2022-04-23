#include "../include/sms.h"

const std::string SMSClient::host = "smtp.qq.com";
const std::string SMSClient::port = "25";

using namespace boost::chrono;
inline bool is_in_str(std::string& main,std::string sub)
{
    return (main.find(sub)!=std::string::npos);
}
SMSClient::SMSClient(boost::asio::io_context& ioc)
    :sockClient_(ioc),
    ioc_(ioc),
    Connecting_(false),
    timeout_(false)
{
    //sockClient_.non_blocking(true); //设置套接字非阻塞
}

SMSClient::~SMSClient()
{
}

bool SMSClient::workThread_login(Req& conn)    //登录流程
{

    
    std::string msg = "ehlo hello\r\n";
    workThread_sendline(msg);
    std::string s;
    workThread_recvive(s);


    msg = "auth login\r\n";
    workThread_sendline(msg);
    workThread_recvive(s);
    if(!is_in_str(s,"334")){
        DEBUG("auth login failed");
        return false;
    }

    msg = base64(conn.senduser);
    msg+="\r\n";
    workThread_sendline(msg);   //账号
    workThread_recvive(s);
    if(!is_in_str(s,"334")){
        DEBUG("login user failed");
        return false;
    }

    msg = base64(conn.sqm);
    msg+="\r\n";
    workThread_sendline(msg);   //授权码
    workThread_recvive(s);
    if(!is_in_str(s,"235")){
        DEBUG("authorization code failed :");
        return false;
    }

    msg = "ehlo hello\r\n";
    workThread_sendline(msg);   
    workThread_recvive(s);
    
    return true;
}




bool SMSClient::workThread_sendData(Req& conn)
{
    std::string msg="";
    std::string s="";

    //发送者
    msg = "mail from:<"+conn.senduser+">\r\n";
    workThread_sendline(msg);   
    workThread_recvive(s);
    if(!is_in_str(s,"250")){
        FATAL("Format error");
        return false;
    }

    //接收者
    msg = "rcpt to:<"+conn.recvuser+">\r\n";
    workThread_sendline(msg);
    workThread_recvive(s);
    if(!is_in_str(s,"250")){
        FATAL("Format error");
        return false;
    }

    //提示开始传输数据
    msg = "DATA\r\n";
    workThread_sendline(msg);
    workThread_recvive(s);
    if(!is_in_str(s,"354")){
        FATAL("Format error");
        return false;
    }

    //传输正文  一行一行传输
    msg = conn.Data+"\r\n";
    workThread_sendline(msg);
    //传输结束
    msg = "\r\n.\r\n";
    s = workThread_sendline(msg);
    workThread_recvive(s);
    if(!is_in_str(s,"queue")){
        FATAL("Format error");
        return false;
    }

    return true;
}

bool SMSClient::workThread_relogin(Req&conn)  //重新登录
{
    return workThread_login(conn);
}


int SMSClient::workThread_recvive(std::string& ret)
{
    char p[1024];
    int n =sockClient_.receive(boost::asio::buffer(p));
    ret = p;
    INFO("[recv] %s",ret.c_str());
    return n;
}


size_t SMSClient::workThread_sendline(std::string&line) //发送一行
{
    int nbyte_to_send = 0;
    while(nbyte_to_send != line.size())
    {
        nbyte_to_send+=sockClient_.send(boost::asio::buffer(std::string(line.begin()+nbyte_to_send,line.end())));
        //INFO("[send] bytes_num: %d",nbyte_to_send);
    }
    INFO("[send] %s",line.c_str());
    return nbyte_to_send;
}
//重新设置超时时间
#define TimerReSet(th,timer,t)  \
{\
    timer.cancel();\
    boost::system::error_code err;\
    timer.async_wait(std::bind([th](boost::system::error_code& err){\
        if(err)\
            ERROR("%s",err.message().c_str());\
            th->timeout_.exchange(true);\ 
    },err));\
}\ 

// inline void TimerReSet(SMSClient* th,boost::asio::steady_timer& timer,time_t t)
// {
//     //如果有消息，处理并重置超时时间
//     timer.cancel();
//     boost::system::error_code err;
//     timer.async_wait(std::bind([th](boost::system::error_code& err){
//         if(err)
//             ERROR("%s",err.message().c_str());
//             th->timeout_.exchange(true);//通知超时
//     },err));
// }

void SMSClient::WorkThread_main(SMSClient* th)
{
    //一、连接远端 smtp.qq.com
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("157.148.54.34"),25);
    th->sockClient_.connect(ep);
    INFO("smtp.qq.com connection is success!");
    th->Connecting_.exchange(true);
    std::string s;
    th->workThread_recvive(s);

    int t = 60; //超时时间
    boost::asio::steady_timer timer(th->ioc_,boost::asio::chrono::seconds(t)); 

    //直到超时60s没有消息退出
    while(true)
    {    
        if(!th->mq_.empty())
        {
            TimerReSet(th,timer,t);

            Req once;   //取出一个req处理
            {
                boost::lock_guard<boost::mutex> lock(th->lock_);
                once = th->mq_.front();
                th->mq_.pop();  //出队
            }

            //执行
            if(th->workThread_login(once))
            {
                INFO("登录流程成功");
                //开始发送
                if(th->workThread_sendData(once))
                {
                    INFO("发送成功! json数据:%s",once.GetStr());
                }
                else
                {
                    INFO("发送失败! json数据:%s",once.GetStr());
                }
            }
            else{
                ERROR("登录流程失败");
                continue;
            }

        }//mq空

        if(th->timeout_)
        {
            INFO("timeout:%d workthread over",t);
            break;
        }
    }


    //即将退出，关闭连接
    s = "quit\r\n";
    th->workThread_sendline(s);
    th->workThread_recvive(s);
    th->close();

}
void SMSClient::close()
{
    sockClient_.close();
    Connecting_.exchange(false);
}

std::string SMSClient::pushAJson(std::string& json)
{    
    {
        boost::lock_guard<boost::mutex> lock(lock_);
        Req tmp;
        if(!json_handle(json,tmp))
        {//解析失败
            FATAL("json format error");
            return "400 bad json";
        }
        else//成
        {
            if(!Connecting_)//work是否运行
                boost::thread(std::bind(SMSClient::WorkThread_main,this)).detach();
            mq_.push(tmp);
            DEBUG("get a req success:%s",tmp.GetStr().c_str());
        }
        return "200 OK";
    }
}


/*
{
    "senduser":"",
    "recvuser":"",
    "sqm":"",
    "data":""
}
*/


//json解析为Req
bool SMSClient::json_handle(std::string& GetRes,Req& tmp)
{
    Json::Reader reader;
    Json::Value root;

    if(!reader.parse(GetRes,root))
    {//解析失败
        return false;
    }
    tmp.senduser = root["senduser"].asString();
    tmp.recvuser = root["recvuser"].asString();
    tmp.sqm = root["sqm"].asString();
    tmp.Data = root["data"].asString();
    if(!tmp.checking()){
        return false;
    }
    return true;
}

using namespace std;
// base64 加密函数
string SMSClient::base64(string& str)   //base64加密算法
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
/*

ip: 43.138.37.7 
port: 13000

{"senduser":"登录smtp需要的邮箱","recvuser":"接收者邮箱","sqm":"需要去qq空间申请的授权吗","data":"subject:主题（直接写在主题就可,内容和主题分离还没有做）\r\n"}

{"senduser":"979336542@qq.com","recvuser":"979336542@qq.com","sqm":"bsgspukoekiqbegd","data":"subject:nia\r\n"}

{
    "senduser":"",
    "recvuser":"",
    "sqm":"",
    "data":""
}   

*/