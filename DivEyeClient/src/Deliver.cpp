#include "Deliver.h"
#include "conf.h"
using namespace std;
int sendInfo(std::string addr, std::map<std::string, std::string> items)
{
    std::cout << "This is client" << std::endl;
    // socket
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1)
    {
        std::cout << "Error: socket" << std::endl;
        return 0;
    }
    // connect
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1234);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.91.154");
    if (connect(client, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cout << "Error: connect" << std::endl;
        return 0;
    }
    std::cout << "...connect" << std::endl;
    char data[255];
    char buf[255];
    while (true)
    {
        std::cin >> data;
        send(client, data, strlen(data), 0);
        if (strcmp(data, "exit") == 0)
        {
            std::cout << "...disconnect" << std::endl;
            break;
        }
        memset(buf, 0, sizeof(buf));
        int len = recv(client, buf, sizeof(buf), 0);
        buf[len] = '\0';
        std::cout << buf << std::endl;
    }
    close(client);
    return 0;
}

map<string, string> receiveInfo(string addrs)
{
    map<string, string> result;
    std::cout << "This is server" << std::endl;
    // socket
    int listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == -1)
    {
        std::cout << "Error: socket" << std::endl;
        return result;
    }
    // bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenSock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        std::cout << "Error: bind" << std::endl;
        return result;
    }
    // listen
    if (listen(listenSock, 5) == -1)
    {
        std::cout << "Error: listen" << std::endl;
        return result;
    }
    // accept
    int conn;
    char clientIP[INET_ADDRSTRLEN] = "";
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    std::cout << "...listening" << std::endl;
    conn = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (conn < 0)
    {
        std::cout << "Error: accept" << std::endl;
        return result;
    }
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    std::cout << "...connect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

    char buf[512];
    string data,token;
    while (token!="EOF")
    {
        string first,second;
        memset(buf, 0, sizeof(buf));
        int len = recv(conn, buf, sizeof(buf), 0);
        if(buf[len-1] ==' '){
            buf[len - 1] = '\0';
        }
        data = buf;
        istringstream iss(data);
        while(iss>>token&&token!="EOF"){
            spiltKV(token, first, second);
            result[first] = second;
        }
    }
    close(conn);
    close(listenSock);
    return result;
}
void spiltKV(std::string KV, std::string &K, std::string &V){
    size_t pos=KV.find(":");
    K=KV.substr(0,pos);
    V=KV.substr(pos+1,KV.size());
}