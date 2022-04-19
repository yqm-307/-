#pragma once
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "Logger.h"

class TcpConnection;

typedef boost::asio::ip::tcp tcp;

//typedef boost::function<void(TcpConnection::ConnectionPtr,const boost::system::error_code&)> OnConnectCallback;
#define YDURL "http://api.guqb.cn/music/yx/qq.php"


