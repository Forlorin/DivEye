#include "Analyze.h"
#include "DBUtil.h"
using namespace std;
void Analyze::save(map<string, string> items)
{
    ofstream saveFile;
    saveFile.open("save.log", std::ios::app | std::ios::out);
    saveFile << "#\n"
             << items["time"] << '\n';
    saveFile << items["cpu_usage"] << ' '
             << items["total_ram"] << ' '
             << items["free_ram"] << '\n';
    saveFile.close();
}
map<string, string> Analyze::load()
{
    map<string, string> items;
    ifstream saveFile;
    saveFile.open("save.log", std::ios::in);
    string tag;
    saveFile >> tag;
    if (tag == "#")
    {
        while (!saveFile.eof())
        {
            saveFile >> items["time"] >> items["cpu_usage"] >> items["total_ram"] >> items["free_ram"];
            saveFile >> tag;
            if (tag == "#")
            {
                continue;
            }
        }
    }
    saveFile.close();
    return items;
}

map<string, string> Analyze::str2map(string str)
{
    map<string, string> items;
    string token, first, second;
    istringstream iss(str);
    while (iss >> token)
    {
        spiltKV(token, first, second);
        items[first] = second;
    }
    return items;
}

map<string, string> Analyze::str2map(string str, string end)
{
    map<string, string> items;
    string token, first, second;
    istringstream iss(str);
    while (iss >> token && token != end)
    {
        spiltKV(token, first, second);
        items[first] = second;
    }
    return items;
}

string Analyze::map2str(map<string, string> items)
{
    string result;
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        if (it != items.begin())
        {
            result += ' ';
        }
        result += it->first + ':' + it->second;
    }
    return result;
}

void Analyze::spiltKV(string KV, string &K, string &V)
{
    size_t pos = KV.find(":");
    K = KV.substr(0, pos);
    V = KV.substr(pos + 1, KV.size());
}