# SMS Server
使用libcurl实现的短信转发微服务

## 搭建流程
    依赖boost、libCurl
    首先安装boost、libcurl

### boost安装    
    wget下载安装包，boost包地址（自己选取合适版本，最新就行）：https://www.boost.org/users/download/
    boost下载完，解压、编译、生成lib文件

### libcurl
    可以用apt-get或yum直接安装

### 下载源码 
    git clone 项目地址
### 安装并编译
    ./build.sh
    或
    bash build.sh
### 执行可执行文件  
    可执行文件在bin目录下
    ./server 即可运行

### 测试
    目前搭了一个测试用的服务器，用法很简单，发json就行。请勿攻击，毕竟我只是个fw罢了QaQ.
    ip: 43.138.37.7 13000
    发送格式
    "{  
        "senduser":"登录smtp需要的邮箱",
        "recvuser":"接收者邮箱",
        "sqm":"需要去qq空间申请的授权吗",
        "data":"subject:主题（直接写在主题就可,内容和主题分离还没有做）\r\n"
    }"