#include "httpd.h"
using namespace std;
DivHttpdServer::DivHttpdServer()
{
    router.Static("/", "./html");
    router.Proxy("/proxy/", "http://httpbin.org/");
    // 使用post方法获得client信息
    router.POST("/FrqInfo", [](HttpRequest *req, HttpResponse *resp)
                {
                   FrqEye eye;
                   eye.clientInit();
                   eye.check();
                   return resp->String(eye.getStringItems()); });

    router.POST("/UserInfo", [](HttpRequest *req, HttpResponse *resp)
                {
                    FrqEye eye;
                    eye.clientInit();
                    eye.checkUser();
                    return resp->String(eye.getStringItems()); });
    router.POST("/AllInfo", [](HttpRequest *req, HttpResponse *resp)
                {
                    FrqEye eye;
                    eye.clientInit();
                    eye.check();
                    eye.checkUser();
                    return resp->String(eye.getStringItems()); });
    router.POST("/Heartbeat", [](HttpRequest *req, HttpResponse *resp)
               { 
                    FrqEye eye;
                    eye.clientInit();
                    return resp->String(eye.nodeName);; });
    
    // 使用GET方法获得客户端信息
    router.GET("/FrqInfoGet", [](HttpRequest *req, HttpResponse *resp)
               {
                   FrqEye eye;
                   eye.clientInit();
                   eye.check();
                   return resp->String(eye.getStringItems()); });
    
    router.GET("/FrqGPUInfoGet", [](HttpRequest *req, HttpResponse *resp)
               {
                   FrqEye eye;
                   eye.clientInit();
                   eye.checkGPU();
                   return resp->String(eye.getStringItems()); });
    server.port = CLIENT_HTTP_PORT;
    server.service = &router;
}

DivHttpdServer::~DivHttpdServer()
{
    http_server_stop(&server);
}

void DivHttpdServer::start()
{
    server.port = CLIENT_HTTP_PORT;
    http_server_run(&server);
}

void DivHttpdServer::start(int port)
{
    server.port = port;
    http_server_run(&server);
}

DivHttpdClient::DivHttpdClient()
{
}

DivHttpdClient::~DivHttpdClient()
{
}

map<string, string> DivHttpdClient::getClientInfo(string clientAddr, string port)
{
    string url;
    url = url + "http://" + clientAddr + ":" + port + "/FrqInfo";
    auto resp = requests::post(url.data());
    string rStr = resp->body.c_str();
    return Analyze::str2map(rStr);
}

map<string, string> DivHttpdClient::getClientUserInfo(string clientAddr, string port)
{
    string url;
    url = url + "http://" + clientAddr + ":" + port + "/UserInfo";
    auto resp = requests::post(url.data());
    string rStr = resp->body.c_str();
    return Analyze::str2map(rStr);
}

map<string, string> DivHttpdClient::getClientAllInfo(string clientAddr, string port)
{
    string url;
    url = url + "http://" + clientAddr + ":" + port + "/AllInfo";
    auto resp = requests::post(url.data());
    string rStr = resp->body.c_str();
    return Analyze::str2map(rStr);
}

string DivHttpdClient::testConnection(string clientAddr, string port)
{
    string url;
    url = url + "http://" + clientAddr + ":" + port + "/Heartbeat";
    auto resp = requests::post(url.data());
    if (resp != NULL)
    {
        return resp->body.c_str();
    }
    else
    {
        return "";
    }
}