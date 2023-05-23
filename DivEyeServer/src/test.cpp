#include "test.h"
void initAll()
{
    Config *config = initialConfig();
    if (config != nullptr)
    {
        connection *dbc = initialDatabase(config);
        if (dbc != nullptr)
        {
            DBUtil *dbUtil = new DBUtil(dbc);
            dbUtil->initTables();
        }
        else
        {
            cout << "Error in initial database" << endl;
        }
    }
    else
    {
        std::cout << "Config file does not exit" << std::endl;
    }
    if (config)
    {
        delete config;
    }
}

void mainLoop()
{
    Config *config = initialConfig();
    string nodeName;
    vector<string> clients;
    if (config != nullptr)
    {
        string line, token;
        nodeName = config->Read("nodeName", nodeName);
        line = config->Read("clientAddr", line);
        istringstream iss(line);
        while (getline(iss, token, ','))
        {
            clients.push_back(token);
        }
        delete config;
        cout << "Initial success" << '\n';
    }
    else
    {
        std::cout << "Config file does not exit" << std::endl;
        return;
    }
    while (1)
    {
        DivHttpdClient *collector = new DivHttpdClient();
        FrqEye *serverEye = new FrqEye();
        serverEye->init();
        serverEye->check();
        serverEye->checkUser();
        serverEye->saveDB();
        serverEye->setNodeStatus(1);
        for (auto clientAddr : clients)
        {
            string cName = collector->testConnection(clientAddr, to_string(CLIENT_HTTP_PORT));
            if (cName != "")
            {
                map<string, string> clientItems =
                    collector->getClientAllInfo(clientAddr, to_string(CLIENT_HTTP_PORT));
                serverEye->saveDB(cName, clientItems);
                serverEye->setNodeStatus(cName, clientAddr, 1);
            }
            else
            {
                serverEye->setNodeStatus(cName, clientAddr, 0);
            }
        }

        delete serverEye;
        delete collector;
        sleep(60);
    }
}

void startServerEye()
{
    DivHttpdServer server;
    server.start();
}
void test()
{
    thread serverEyeThread(startServerEye),mainLoopThread(mainLoop);
    serverEyeThread.join();
    mainLoopThread.join();
    // FrqEye eye;
    // eye.clientInit();
    // eye.checkGPU();
}
