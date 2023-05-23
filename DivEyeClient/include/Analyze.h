#ifndef ANALYZE_H
#define ANALYZE_H
#include <iostream>
#include <map>
#include <string>
#include <fstream>
class Analyze
{
public:
    static void spiltKV(std::string KV, std::string &K, std::string &V);
    static void save(std::map<std::string, std::string> items);
    static std::map<std::string, std::string> load();
    static std::map<std::string, std::string> str2map(std::string str);//没有休止符的版本，默认传递完全
    static std::map<std::string, std::string> str2map(std::string str,std::string end);//规定了休止符，读到休止符才算读取完全
    static std::string map2str(std::map<std::string, std::string> items);
    
};
#endif