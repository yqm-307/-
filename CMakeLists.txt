cmake_minimum_required(VERSION 2.6)
project(SMSServer CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

#源文件
file(GLOB SOURCE ./src/*.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


include_directories(include)


add_executable(server ${SOURCE})

target_link_libraries(server
    -lboost_regex
    -lboost_thread
    -ljsoncpp
    -lcurl
    -lpthread

)