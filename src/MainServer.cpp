#include "util.h"
#include "Server.h"
#define MAXPARAM 2048


void readJson_Config(std::string file) {
	std::ifstream ifs;
	ifs.open(file, std::ios::in);   //打开json文件
	if (!ifs.is_open())
	{
		FATAL("open json file failed.");
		return;
	}

	Json::CharReaderBuilder reader;
	Json::Value root;

	int iage = 0;
	JSONCPP_STRING errs;

	bool res = Json::parseFromStream(reader, ifs, &root, &errs);
	if (!res || !errs.empty())
	{
		std::cout << "parseJson error! " << errs << std::endl;
		return;
	}

	std::string key = root["key"].asString();
	std::string my = root["my"].asString();
	//std::string ip = root["ip"].asString();
	std::string port = root["port"].asString();
	ifs.close();

	return;
}

int main()
{
    
    boost::asio::io_context ioc;
    Server server(ioc,tcp::endpoint(tcp::v4(),13000));
    server.start();
    //ioc.run();
}