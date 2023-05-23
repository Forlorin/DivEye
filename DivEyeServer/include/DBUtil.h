#ifndef DBUTIL_H
#define DBUTIL_H

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <ctime>

using namespace std;
using namespace pqxx;

class DBUtil
{
private:
    connection *dbConnection;
    string userName;
    string password;

public:
    void initial(string dbname, string userName, string password, string host, string port);
    DBUtil();
    DBUtil(connection *dbConnection);
    ~DBUtil();

public:
    void commitWork(string sql);
    result runQuery(string sql);
    void cleanTable(string tName);
    bool tableExists(string tName);
    void initTables();
};

string getStringTime();
string addQuota(string str);
#endif