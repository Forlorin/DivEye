#include "RecordTool.h"
using namespace std;
bool pidIsInvalid(Process &proc)
{
    return proc.pid == -1;
}
string userProc::toString()
{
    stringstream ss;
    if(!isfinite(cpu)||cpu<0.0001||isnan(cpu)){
        cpu = 0.0;
    }
    ss<<setiosflags(ios::fixed) << setprecision(4) << cpu;
    string str;
    str += to_string(mem) + '|' + ss.str();
    return str;
}