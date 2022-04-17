#include <stdio.h>
#include <curl/curl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <boost/asio.hpp>
#define MAXPARAM 2048

CURL*curl;
CURLcode res;

/* 短信内容 */
char *msg = "【董志科技】您的验证码是：541788";
/* 发送url 请咨询客服 */
char *url = "http://139.196.108.241:8080/Api/HttpSendSMYzm.ashx";

void send_data( char *url, char *data )
{
    /* specify the url */
    curl = curl_easy_init();
    curl_easy_setopt( curl, CURLOPT_URL, url );
    printf( "url:%s\n", url );
    /* specify the POST data */
    curl_easy_setopt( curl, CURLOPT_POSTFIELDS, data );
    printf( "data:%s\n", data );
    /* get response data */
    CURLcode res = curl_easy_perform( curl );
    printf( "%d\n", res );
    /* printf("\n\n"); */
}
/**
 * 发送短信
 */
void send_sms( char *url, char *account, char *pswd, char *mobile, char *msg )
{
    char params[MAXPARAM + 1];
    char*cp = params;
    sprintf( params, "account=%s&pswd=%s&mobile=%s&msg=%s&needstatus=true", account, pswd, mobile, msg );
    send_data( url, params );
}

int main( void )
{
    /* 账号 */
    char *account = "18537780190";
    /* 密码 */
    char *pswd = "Yqm1025889578";
    /* 修改为您要发送的手机号 */
    char *mobile = "18537780190";
    /* 发送验证码短信 */
    send_sms( url, account, pswd, mobile, msg );
    return(0);
}