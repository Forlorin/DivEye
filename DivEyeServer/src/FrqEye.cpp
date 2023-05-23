#ifndef FRQEYE_CPP
#define FRQEYE_CPP
#include "FrqEye.h"
FrqEye::FrqEye()
{
    dbUtil = nullptr;
}

FrqEye::FrqEye(DBUtil *dbUtil)
{
    this->dbUtil = dbUtil;
}

FrqEye::~FrqEye()
{
    if (dbUtil)
    {
        delete dbUtil;
    }
}
// Server端使用的初始化函数
bool FrqEye::init()
{
    Config *config = initialConfig();
    // 调用config读取类来进行配置文件的读取
    if (config != nullptr)
    {
        // 创建数据库链接
        connection *dbc = initialDatabase(config);
        if (dbc != nullptr)
        {
            nodeName = config->Read("nodeName", nodeName);
            serverAddr = config->Read("serverAddr", serverAddr);
            string clientAddrs = config->Read("clientAddr", clientAddrs);
            // 读取client地址
            istringstream iss(clientAddrs);
            string token;
            while (getline(iss, token, ','))
            {
                clientAddr.push_back(token);
            }
            DBUtil *dbUtil = new DBUtil(dbc);
            this->dbUtil = dbUtil;
            dbUtil->initTables();

            string uStr = config->Read("monitorUserList", uStr);
            iss.clear();
            iss.str(uStr);
            while (getline(iss, token, ','))
            {
                string userId, userName;
                Analyze::spiltKV(token, userName, userId);
                int id = stoi(userId);
                if (id != -1)
                {
                    users[id] = userName;
                }
            }
            return true;
        }
        else
        {
            cout << "Error in initial database" << endl;
            return false;
        }
    }

    else
    {
        std::cout << "Config file does not exit" << std::endl;
        return false;
    }
}

// client端使用的初始化函数
bool FrqEye::clientInit()
{
    Config *config = initialConfig();
    if (config != nullptr)
    {
        nodeName = config->Read("nodeName", nodeName);
        serverAddr = config->Read("serverAddr", serverAddr);
        string uStr = config->Read("monitorUserList", uStr);
        string token;
        istringstream iss(uStr);
        while (getline(iss, token, ','))
        {
            string userId, userName;
            Analyze::spiltKV(token, userName, userId);
            int id = stoi(userId);
            if (id != -1)
            {
                users[id] = userName;
            }
        }
        return true;
    }
    else
    {
        std::cout << "Config file does not exit" << std::endl;
        return false;
    }
}

void FrqEye::check()
{
    checkCpu();
    checkMem();
}

// 获取CPU使用率
void FrqEye::checkCpu()
{
    string data;
    ifstream cpuIfo;
    unsigned long long oldData, newData;
    unsigned long long user, nice, system, oldIdle, newIdle, loWait, irq, sof;
    cpuIfo.open("/proc/stat");
    cpuIfo >> data >> user >> nice >> system >> oldIdle >> loWait >> irq >> sof;
    cpuIfo.close();
    oldData = user + nice + system + oldIdle + loWait + irq + sof;
    sleep(1);
    cpuIfo.open("/proc/stat");
    cpuIfo >> data >> user >> nice >> system >> newIdle >> loWait >> irq >> sof;
    cpuIfo.close();
    newData = user + nice + system + newIdle + loWait + irq + sof;
    double sum = newData - oldData, idle = newIdle - oldIdle;
    double cpuUsage = 1.0 - (idle / sum);
    if (!isfinite(cpuUsage) || cpuUsage < 0.0001 || isnan(cpuUsage))
    {
        cpuUsage = 0.0;
    }
    stringstream ss;
    ss << setprecision(4) << cpuUsage;
    data = ss.str();
    items["cpu_usage"] = data;
}

// 获取内存使用率
void FrqEye::checkMem()
{
    struct sysinfo sysInfo;
    // 内存信息的单位为MB
    string tag;
    ifstream memIfo;
    memIfo.open("/proc/meminfo");

    unsigned long totalRam;
    unsigned long freeRam;
    memIfo >> tag >> totalRam >> tag;
    memIfo >> tag >> freeRam;
    memIfo.close();
    items["total_ram"] = to_string(totalRam >> 10);
    items["free_ram"] = to_string(freeRam >> 10);
}

void FrqEye::checkGPU()
{
    nvmlReturn_t result;
    unsigned int device_count, i;
    // First initialize NVML library
    result = nvmlInit();
    result = nvmlDeviceGetCount(&device_count);
    if (NVML_SUCCESS != result)
    {
        std::cout << "Failed to query device count: " << nvmlErrorString(result);
        return;
    }

    for (i = 0; i < device_count; i++)
    {
        nvmlDevice_t device;
        char name[NVML_DEVICE_NAME_BUFFER_SIZE];
        nvmlPciInfo_t pci;
        result = nvmlDeviceGetHandleByIndex(i, &device);
        if (NVML_SUCCESS != result)
        {
            std::cout << "get device failed " << endl;
        }
        result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
        // 使用率
        nvmlUtilization_t utilization;
        result = nvmlDeviceGetUtilizationRates(device, &utilization);
        if (NVML_SUCCESS == result)
        {
            items["GPU" + to_string(i)] = to_string(utilization.gpu);
        }
    }
}

void FrqEye::checkUser()
{
    // 获取进程号
    DIR *procPath = opendir("/proc/");
    if (!procPath)
    {
        return;
    }
    struct dirent *entry;
    vector<Process> procs;
    Process temp;
    while ((entry = readdir(procPath)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            char *endptr;
            long pid = strtol(entry->d_name, &endptr, 10);
            if (*endptr == '\0')
            {
                temp.pid = pid;
                procs.push_back(temp);
            }
        }
    }
    closedir(procPath);

    // 获取进程数据
    int interval = 1;
    string line;
    ifstream user_file;
    // 获取进程的所属用户
    for (auto &proc : procs)
    {
        user_file.open("/proc/" + to_string(proc.pid) + "/status");
        if (user_file.is_open())
        {
            while (getline(user_file, line))
            {
                if (line.substr(0, 5) == "Uid:\t")
                {
                    proc.uid = stoi(line.substr(5));
                    break;
                }
            }
        }
        else
        {
            proc.pid = -1;
        }
        user_file.close();
    }

    // 获取进程的CPU和内存占用信息
    ifstream stat_file, cpuInfo;
    string data;
    unsigned long long oldData, newData;
    unsigned long long user, nice, system, oldIdle, newIdle, loWait, irq, sof;
    cpuInfo.open("/proc/stat");
    cpuInfo >> data >> user >> nice >> system >> oldIdle >> loWait >> irq >> sof;
    oldData = user + nice + system + oldIdle + loWait + irq + sof;
    cpuInfo.close();

    unsigned long long utime_ticks, stime_ticks, rss;
    for (auto &proc : procs)
    {
        stat_file.open("/proc/" + to_string(proc.pid) + "/stat");
        if (stat_file.is_open())
        {
            getline(stat_file, line);

            sscanf(line.c_str(), "%*d %*s %*c "
                                 "%*d %*d %*d %*d %*d "
                                 "%*u %*u %*u %*u %*u %lu %lu %*ld %*d %*d %*d %*d %*d "
                                 "%*lu %*lu %lu",
                   &utime_ticks, &stime_ticks, &rss);
            proc.utime = utime_ticks;
            proc.stime = stime_ticks;
            proc.mem = rss >> 10;
        }
        else
        {
            proc.pid = -1;
        }
        stat_file.close();
    }

    // 计算CPU占用率
    sleep(interval);
    ifstream new_stat_file;
    unsigned long long new_utime_ticks, new_stime_ticks;
    cpuInfo.open("/proc/stat");
    cpuInfo >> data >> user >> nice >> system >> newIdle >> loWait >> irq >> sof;
    newData = user + nice + system + newIdle + loWait + irq + sof;
    for (auto &proc : procs)
    {
        new_stat_file.open("/proc/" + to_string(proc.pid) + "/stat");
        getline(new_stat_file, line);
        sscanf(line.c_str(), "%*d %*s %*c "
                             "%*d %*d %*d %*d %*d "
                             "%*u %*u %*u %*u %*u %lu %lu %*ld %*d %*d %*d %*d %*d "
                             "%*lu %*lu %lu",
               &new_utime_ticks, &new_stime_ticks);
        proc.new_utime = new_utime_ticks;
        proc.new_stime = new_stime_ticks;
        proc.cpu = ((new_utime_ticks - proc.utime) + (new_stime_ticks - proc.stime)) / (double)sysconf(_SC_CLK_TCK * interval);
    }
    procs.erase(remove_if(procs.begin(), procs.end(), pidIsInvalid), procs.end());
    double sysTicket = (double)(oldData - newData);
    for (auto &proc : procs)
    {
        proc.cpu = (double)(proc.new_utime - proc.utime + proc.new_stime - proc.stime) / sysTicket;
    }
    vector<userProc> userProcs;
    userProc tempUserProc;
    tempUserProc.cpu = 0;
    tempUserProc.mem = 0;
    tempUserProc.uid = -1;
    for (auto &it : users)
    {
        tempUserProc.uid = it.first;
        userProcs.push_back(tempUserProc);
    }

    // 对于每一个进程
    for (auto &proc : procs)
    {
        // 查看是否为要监视的用户
        for (auto &up : userProcs)
        {
            if (proc.uid == up.uid)
            {
                // 查看是否已经存储，如果有，在已存储的用户信息中加上用户使用的
                // 如果没有，那么则新建一个
                if (userProcs.size() != 0)
                {
                    bool notFind = true;
                    for (auto &iter : userProcs)
                    {
                        if (iter.uid == proc.uid)
                        {
                            iter.mem += proc.mem;
                            iter.cpu += proc.cpu;
                            notFind = 0;
                            break;
                        }
                    }
                    if (notFind)
                    {
                        tempUserProc.uid = proc.uid;
                        tempUserProc.mem = proc.mem;
                        tempUserProc.cpu = proc.cpu;
                        userProcs.push_back(tempUserProc);
                    }
                }
                else
                {
                    tempUserProc.uid = proc.uid;
                    tempUserProc.mem = proc.mem;
                    tempUserProc.cpu = proc.cpu;
                    userProcs.push_back(tempUserProc);
                }
                break;
            }
        }
    }
    for (auto &it : userProcs)
    {
        // cout << it.cpu << '\n';
        items["userFrq" + to_string(it.uid)] = it.toString();
    }
}

void FrqEye::save()
{
    Analyze::save(items);
}

void FrqEye::load()
{
    map<string, string> nItems;
    items["total_ram"] = nItems["total_ram"];
    items["cpu_usage"] = nItems["cpu_usage"];
    items["free_ram"] = nItems["free_ram"];

    cout << "cpu_usage:" << items["cpu_usage"] << '\n'
         << "free_ram:" << items["free_ram"] << '\n'
         << "total_ram" << items["total_ram"] << '\n';
}

void FrqEye::saveDB()
{
    string sql = CpuInfo::insert(getStringTime(), nodeName, items["cpu_usage"]);
    dbUtil->commitWork(sql);
    sql = MemInfo::insert(getStringTime(), nodeName, items["total_ram"], items["free_ram"]);
    dbUtil->commitWork(sql);
    for (auto it : items)
    {
        if (it.first.substr(0, 7) == "userFrq")
        {
            string uid = it.first.substr(7);
            int pos = it.second.find('|');
            userUse::insert(dbUtil,
                            getStringTime(),
                            nodeName,
                            uid,
                            it.second.substr(0, pos),
                            it.second.substr(pos + 1));
        }
    }
    // result r = MemInfo::selectAll(dbUtil);
    // for (result::const_iterator it = r.begin(); it != r.end(); it++)
    // {
    //     cout
    //         << it[0].as<string>() << '\t'
    //         << it[1].as<string>() << '\t'
    //         << it[2].as<string>() << '\t'
    //         << it[3].as<string>() << endl;
    // }
}

void FrqEye::saveDB(string cnn, map<string, string> ci)
{
    dbUtil->commitWork(CpuInfo::insert(getStringTime(), cnn, ci["cpu_usage"]));
    dbUtil->commitWork(MemInfo::insert(getStringTime(), cnn, ci["total_ram"], items["free_ram"]));
    for (auto it : ci)
    {
        if (it.first.substr(0, 7) == "userFrq")
        {
            string uid = it.first.substr(7);
            int pos = it.second.find('|');
            userUse::insert(dbUtil,
                            getStringTime(),
                            cnn,
                            uid,
                            it.second.substr(0, pos),
                            it.second.substr(pos + 1));
        }
    }
}

map<string, string> FrqEye::getItems()
{
    return items;
}

// 从客户端获取数据
void FrqEye::giveItem(map<string, string> item)
{
    for (auto it : item)
    {
        items[it.first] = it.second;
    }
}

string FrqEye::getStringItems()
{
    return Analyze::map2str(items);
}

void FrqEye::printItems()
{
    for (auto it : items)
    {
        cout << it.first << ':' << it.second << '\n';
    }
}

void FrqEye::setNodeStatus(string nodeName, string nodeAddr, int status)
{
    int oldStatus = nodeStatus::queryStatus(dbUtil, nodeAddr);

    if (oldStatus == 1)
    { // 该节点之前为active,不论现在是什么状态，更新
        nodeStatus::updateStatus(dbUtil, nodeName,nodeAddr, status, getStringTime());
    }
    else if (oldStatus == 0)
    { // 该结点之前宕机，则检查
        if (status == 1)
        {
            nodeStatus::updateStatus(dbUtil,nodeName,nodeAddr, status, getStringTime());
        } // 如果仍然为宕机，则不改变
    }
    else if (oldStatus == -1)
    {
        nodeStatus::insert(dbUtil, nodeName, nodeAddr, status, getStringTime());
    }
}

void FrqEye::setNodeStatus(int status)
{

    int oldStatus = nodeStatus::queryStatus(dbUtil, serverAddr);

    if (oldStatus == 1)
    { // 该节点之前为active,不论现在是什么状态，更新
        nodeStatus::updateStatus(dbUtil, nodeName,serverAddr, status, getStringTime());
    }
    else if (oldStatus == 0)
    { // 该结点之前宕机，则检查
        if (status == 1)
        {
            nodeStatus::updateStatus(dbUtil, nodeName,serverAddr, status, getStringTime());
        } // 如果仍然为宕机，则不改变
    }
    else if (oldStatus == -1)
    {
        nodeStatus::insert(dbUtil, nodeName, serverAddr, status, getStringTime());
    }
}
#endif