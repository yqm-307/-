#!/bin/bash

clear="$*" #获取环境变量

clear()
{
    rm -rf CMakeFiles
    rm *cmake*
    rm CMakeCache.txt
    rm Makefile
    rm -rf bin
    rm log.txt
}

build()
{
    #编译整个项目目录
    mkdir bin
    cmake .
    make
    
    #编译子目录
    # FILE=./test
    # if [ -d "$FILE" ]; then
    #     #存在
    #     cd test
    #     cmake .
    #     make
    #     cd ..
    # fi
}

if [ "${clear}" = "clear" ]
then
    clear
    #cd test
    #./clear.sh
else
    build
fi