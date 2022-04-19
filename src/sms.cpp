#include "../include/sms.h"

SMSClient::SMSClient(boost::asio::io_context& ioc)
{
    curl_ = curl_easy_init();
    curl_easy_setopt(curl_,CURLOPT_URL,YDURL);  //使用游动网络的url

}

SMSClient::~SMSClient()
{
    curl_easy_cleanup(curl_);
}

int SMSClient::pushAJson(std::string& json)
{
    makeGetUrl(json);
    datas_.push_back(json);
}


/*
    json格式
    {
        "yzm" : String  验证码
        "jsz" : String  接收者
        "bt" : String   标题
        "nr" : String   内容
        "zbt" : String  主标题
    }
*/
int SMSClient::makeGetUrl(std::string& GetRes)
{
    Json::Reader reader;
    Json::Value root;

    int ret = reader.parse(GetRes,root);

    std::string yzm = root["yzm"].asString();
    std::string jsz = root["jsz"].asString();
    std::string bt = root["bt"].asString();
    std::string nr = root["nr"].asString();
    std::string zbt = root["zbt"].asString();

    //生成 http Get请求
    //std::string res = "Get http://api.guaqb.cn/music/yxkey.php?key=&my=email=接收者邮箱&bt=标题&nr=邮箱内容&nr=主标题 HTTP/1.1";
    std::string ret = format("Get\thttp://api.guaqb.cn/music/yxkey.php?key=%s&my=%semail=%s&bt=%s&nr=%s&nr=%s\tHTTP/1.1\n\t",key_,my_,jsz,bt,nr,zbt);
    

}
