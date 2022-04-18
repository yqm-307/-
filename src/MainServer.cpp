#include <stdio.h>
#include <curl/curl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "Server.h"
#define MAXPARAM 2048



int main()
{
    boost::asio::io_context ioc;
    Server server(ioc,tcp::endpoint(tcp::v4(),13000));
    server.start();
    
}