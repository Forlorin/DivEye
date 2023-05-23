#ifndef TABLES_H
#define TABLES_H
#include <pqxx/pqxx>
#include "DBUtil.h"
class CpuInfo
{
private:
    std::string time;
    std::string node;
    float cpuuse;

public:
    static std::string insert(std::string time, std::string node, std::string cpuuse);
    static pqxx::result selectAll(DBUtil *db);
};

class MemInfo
{
private:
    std::string time;
    std::string node;
    std::string totalram;
    std::string freeram;

public:
    static std::string insert(std::string time, std::string node, std::string totalram, std::string freeram);
    static pqxx::result selectAll(DBUtil *db);
};

class userInfo
{
private:
    int id;
    std::string name;

public:
    static std::map<int, std::string> selectAll(DBUtil *db);
    static void deleteAll(DBUtil *db);
    static int queryId(std::string name, DBUtil *db);
};

class userUse
{
private:
    std::string time;
    std::string node;
    int uid;
    double cpuuse;
    double ramuse;

public:
    static void insert(DBUtil *db, std::string time, std::string node, std::string uid, std::string ramuse, std::string cpuuse);
};

class nodeStatus
{
    std::string name;
    std::string addr;
    int status;
    std::string time;

public:
    static void deleteAll(DBUtil *db);
    static void insert(DBUtil *db, std::string name, std::string addr, int status, std::string time);
    static int queryStatus(DBUtil *db, std::string addr);
    static void updateStatus(DBUtil *db, std::string addr, int status, std::string time);
    static void updateStatus(DBUtil *db, std::string name, std::string addr, int status, std::string time);
};
#endif