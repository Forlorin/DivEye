#ifndef FRQEYE_H
#define FRQEYE_H
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <vector>
#include "DBUtil.h"
#include "initial.h"
#include "Tables.h"
#include "Analyze.h"
#include "RecordTool.h"
#include "nvml.h"
class FrqEye
{
private:
    std::map<std::string, std::string> items;
    DBUtil *dbUtil;

public:
    std::string nodeName;
    std::string serverAddr;
    std::vector<std::string> clientAddr;
    std::map<int, std::string> users;

public:
    FrqEye();
    FrqEye(DBUtil *dbUtil);
    ~FrqEye();

    std::map<std::string, std::string> getItems();
    std::string getStringItems();
    bool init();
    bool clientInit();
    void check();
    void save();
    void load();
    void saveDB();
    void saveDB(std::string clientNodeName, std::map<std::string, std::string> clientItems);
    void checkCpu();
    void checkMem();
    void checkUser();
    void checkGPU();
    void giveItem(std::map<std::string, std::string> item);
    void printItems();
    void setNodeStatus(std::string nodeName, std::string nodeAdder, int status);
    void setNodeStatus(int status);
    
};

#endif