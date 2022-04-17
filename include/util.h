#include <curl/curl.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

typedef boost::asio::ip::tcp tcp;

typedef boost::function<void(tcp::endpoint&)> OnConnectCallback;

class util
{
public:
    util(): 
    curl(curl_easy_init())
    {/*log*/}

    ~util(){
        curl_easy_cleanup(curl);
    }

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
        errcode = curl_easy_perform( curl );
        printf( "%d\n", errcode );
        /* printf("\n\n"); */
    }

private:
    CURL* curl;
    CURLcode errcode;
};

