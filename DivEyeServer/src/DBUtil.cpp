#include "DBUtil.h"

void DBUtil::initial(string dbname, string userName, string password, string host, string port)
{
    string connectWord = "dbname=" + dbname +
                         " user=" + userName +
                         " password=" + password +
                         " hostaddr=" + host +
                         " port=" + port;
    try
    {
        dbConnection = new connection();
        if (dbConnection->is_open())
        {
            cout << "DB connection opened" << endl;
        }
        else
        {
            cout << "DB connection failed" << endl;
        }
    }
    catch (const std::exception &e)
    {
        cerr << e.what() << std::endl;
    }
}

DBUtil::DBUtil()
{
    initial("diveye", "diveyeuser", "diveyeuser", "127.0.0.1", "4532");
}

DBUtil::DBUtil(connection *dbc)
{
    dbConnection = dbc;
}

DBUtil::~DBUtil()
{
    dbConnection->close();
    delete dbConnection;
}

void DBUtil::commitWork(string sql)
{
    work W(*dbConnection);
    W.exec(sql);
    W.commit();
}

result DBUtil::runQuery(string sql)
{
    nontransaction N(*dbConnection);
    result R = N.exec(sql);
    return R;
}

void DBUtil::cleanTable(string tName)
{
    string sql = "DELETE FROM " + tName + ";";
    commitWork(sql);
}

bool DBUtil::tableExists(string tName)
{
    string query = "select count(*) from pg_class where relname = '" + tName + "';";
    nontransaction N(*dbConnection);
    result r(N.exec(query));
    if (r[0][0].as<int>() == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void DBUtil::initTables()
{
    if (!tableExists("cpuinfo"))
    {
        string work =
            "CREATE TABLE cpuinfo(\
                id serial PRIMARY KEY  NOT NULL,\
                time text NOT NULL, \
                node text NOT NULL, \
                cpuuse real NOT NULL \
            );";
        commitWork(work);
    }
    if (!tableExists("meminfo"))
    {
        string work =
            "CREATE TABLE meminfo(\
                id serial PRIMARY KEY  NOT NULL,\
                time text NOT NULL, \
                node text NOT NULL, \
                totalram real NOT NULL, \
                freeram real NOT NULL\
                );";
        commitWork(work);
    }
    if (!tableExists("clientinfo"))
    {
        string work =
            "CREATE TABLE clientinfo(\
                id serial PRIMARY KEY  NOT NULL,\
                name text NOT NULL, \
                ipaddr text NOT NULL \
                );";
        commitWork(work);
    }
    if (!tableExists("userinfo"))
    {
        string sql =
            "CREATE TABLE userinfo(\
                id int PRIMARY KEY  NOT NULL,\
                name text NOT NULL \
                );";
        commitWork(sql);
        sql = "insert into userInfo values (0,\'root\');";
        commitWork(sql);
    }
    if (!tableExists("useruse"))
    {
        string sql =
            "CREATE TABLE useruse(\
                id serial PRIMARY KEY  NOT NULL,\
                time text NOT NULL, \
                node text NOT NULL, \
                uid int NOT NULL, \
                cpuuse real NOT NULL, \
                ramuse real NOT NULL\
                );";
        commitWork(sql);
    }
    if (!tableExists("nodestatus"))
    {
        string sql =
            "CREATE TABLE nodestatus(\
                name text NOT NULL,\
                addr text PRIMARY KEY NOT NULL, \
                status int NOT NULL,\
                time text NOT NULL\
                );";
        commitWork(sql);
    }
}

string getStringTime()
{
    time_t now = time(NULL);
    tm *t = localtime(&now);
    string year = to_string(t->tm_year + 1900),
           month = to_string(t->tm_mon + 1),
           day = to_string(t->tm_mday),
           hour = to_string(t->tm_hour),
           minute = to_string(t->tm_min);
    if (month.length() < 2)
    {
        month = "0" + month;
    }
    if (day.length() < 2)
    {
        day = "0" + day;
    }
    if (hour.length() < 2)
    {
        hour = "0" + hour;
    }
    if (minute.length() < 2)
    {
        minute = "0" + minute;
    }
    return year +'-'+ month +'-'+ day +'|'+ hour +':'+ minute;
}

string getStringTime(char mode)
{
    time_t now = time(NULL);
    tm *t = localtime(&now);
    string year = to_string(t->tm_year + 1900),
           month = to_string(t->tm_mon + 1),
           day = to_string(t->tm_mday),
           hour = to_string(t->tm_hour),
           minute = to_string(t->tm_min),
           second = to_string(t->tm_sec);
    if (month.length() < 2)
    {
        month = "0" + month;
    }
    if (day.length() < 2)
    {
        day = "0" + day;
    }
    if (hour.length() < 2)
    {
        hour = "0" + hour;
    }
    if (minute.length() < 2)
    {
        minute = "0" + minute;
    }
    if (second.length() < 2)
    {
        second = "0" + second;
    }
    if (mode == 's')
    {
        return year + month + day + hour + minute + second;
    }
    else
    {
        return year + month + day + hour + minute;
    }
}

string addQuota(string str)
{
    return '\'' + str + '\'';
}