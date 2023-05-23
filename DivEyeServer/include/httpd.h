#ifndef HTTPD_H
#define HTTPD_H
#include <hv/hv.h>
#include <string>
#include <hv/HttpServer.h>
#include <hv/requests.h>
#include <map>
#include "FrqEye.h"
#include "conf.h"
#include "Analyze.h"
//diveye 客户启动的，负责提供客户端查询的服务
class DivHttpdServer
{
private:
    HttpService router;
    http_server_t server;

public:
    DivHttpdServer();
    ~DivHttpdServer();
    void start();
    void start(int port);
};

//DivEye服务端启动的，负责获取客户端的服务
class DivHttpdClient
{
private:
public:
    DivHttpdClient();
    ~DivHttpdClient();
    std::map<std::string, std::string> getClientInfo(std::string clientAddr,std::string port);
    std::map<std::string, std::string> getClientUserInfo(std::string clientAddr,std::string port);
    std::map<std::string, std::string> getClientAllInfo(std::string clientAddr,std::string port);
    std::string testConnection(std::string clientAddr,std::string port);
};

#endif