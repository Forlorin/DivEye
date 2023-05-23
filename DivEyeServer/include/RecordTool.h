#ifndef RECORD_TOOL_H
#define RECORD_TOOL_H
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>

class RecordTool
{
private:
public:
    RecordTool();
    ~RecordTool();
};

class Process
{
public:
    int pid;
    int uid;
    unsigned long long mem;
    float cpu;
    unsigned long long utime;
    unsigned long long stime;
    unsigned long long new_utime;
    unsigned long long new_stime;
    void toString()
    {

        std::cout << "pid:" << pid << '\n'
             << "uid:" << uid << '\n'
             << "mem:" << mem << '\n'
             << "cpu:" << cpu << '\n';
    }
};
bool pidIsInvalid(Process &proc);

class userProc
{
public:
    int uid;
    unsigned long long mem;
    float cpu;
void outPut(){
        std::cout << "uid:" << uid << '\n'
             << "mem:" << mem << '\n'
             << "cpu:" << cpu << '\n';
}
std::string toString();
};

#endif
