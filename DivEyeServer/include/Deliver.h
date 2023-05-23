#ifndef DELIVER_H
#define DELIVER_H
#include <string>
#include <map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
int sendInfo(std::string addr, std::map<std::string, std::string> items);
std::map<std::string, std::string> receiveInfo(std::string addr);
void spiltKV(std::string KV, std::string &K, std::string &V);
#endif