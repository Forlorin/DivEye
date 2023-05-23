#include "Tables.h"
using namespace std;
using namespace pqxx;
string CpuInfo::insert(string time, string node, string cpuuse)
{
    return to_string("INSERT INTO cpuinfo (time,node,cpuuse) VALUES( ") +
           '\'' + time + "\', " +
           '\'' + node + "\', " +
           cpuuse + ");";
}

result CpuInfo::selectAll(DBUtil *db)
{
    return db->runQuery("SELECT * FROM cpuinfo ;");
}

string MemInfo::insert(string time, string node, string totalram, string freeram)
{
    return to_string("INSERT INTO meminfo (time,node,totalram,freeram) VALUES( ") +
           "\'" + time + "\', " +
           "\'" + node + "\', " +
           totalram + ", " +
           freeram + ");";
}

result MemInfo::selectAll(DBUtil *db)
{
    return db->runQuery("SELECT * FROM meminfo;");
}

map<int, string> userInfo::selectAll(DBUtil *db)
{
    result r = db->runQuery("SELECT * FROM userinfo;");
    map<int, string> res;
    for (auto it : r)
    {
        res[it[0].as<int>()] = it[1].as<string>();
    }
    return res;
}

void userInfo::deleteAll(DBUtil *db)
{
    db->commitWork("DELETE * FROM userinfo;");
}

int userInfo::queryId(string name, DBUtil *db)
{
    string sql = to_string("select id from userinfo where name =") + '\'' + name + "\';";
    result r = db->runQuery(sql);
    if (r.empty())
    {
        return -1;
    }
    else
    {
        return r[0][0].as<int>();
    }
}

void userUse::insert(DBUtil *db, string time, string node, string uid, string ramuse, string cpuuse)
{
    db->commitWork(to_string("INSERT INTO useruse (time,node,uid,cpuuse,ramuse) VALUES( ") +
                   '\'' + time + "\', " +
                   '\'' + node + "\', " +
                   uid + "," +
                   cpuuse + "," +
                   ramuse + ");");
}

void nodeStatus::deleteAll(DBUtil *db){
    db->commitWork("DELETE * FROM nodestatus");
}

void nodeStatus::insert(DBUtil *db, string name, string addr, int status, string time)
{
    db->commitWork(to_string("INSERT INTO nodestatus (name,addr,status,time) VALUES( ") +
                   '\'' + name + "\', " +
                   '\'' + addr + "\', " +
                   to_string(status) + "," +
                   '\'' + time + "\');");
}
int nodeStatus::queryStatus(DBUtil *db, string addr)
{
    auto result = db->runQuery(to_string("SELECT status FROM nodestatus WHERE addr = ") +
                               '\'' + addr + "\';");
    if (result.empty())
    {
        return -1;
    }
    else
    {
        return result[0][0].as<int>();
    }
}

void nodeStatus::updateStatus(DBUtil *db, string addr, int status, string time)
{
    db->commitWork(
        to_string("UPDATE nodestatus SET ") +
        "status = " + to_string(status) + "," +
        "time = " + '\'' + time + "\' " +
        " WHERE addr = " + '\'' + addr + "\';");
}
void nodeStatus::updateStatus(DBUtil *db,string name, string addr, int status, string time)
{
    db->commitWork(
        to_string("UPDATE nodestatus SET ") +
        "name = " + '\'' + name + "\' " + "," +
        "status = " + to_string(status) + "," +
        "time = " + '\'' + time + "\' " +
        " WHERE addr = " + '\'' + addr + "\';");
}