#include "../include/sms.h"

SMSClient::SMSClient(boost::asio::io_context& ioc)
{
    curl_ = curl_easy_init();
    curl_easy_setopt(curl_,CURLOPT_URL,YDURL);  //使用游动网络的url

}