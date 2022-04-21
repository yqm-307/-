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
