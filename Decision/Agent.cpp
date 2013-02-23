/*
 * Agent.cpp
 *
 *  Created on: Dec 17, 2009
 *      Author: Mehran Akhavan
 */

#include "Agent.h"
#include "../Utilities/Basics.h"
#include "OfflineConnection.h"
#include <iostream>


using namespace std;
using namespace basics;
using namespace Types;


#define LOGLEVEL 1

Agent::Agent()
{
    this->id = -1;
}

void Agent::initializeConfig()
{
    string configTopic;
    string fileCounterValue;
    config.add("Config_FileAdresses");
    config["Config_FileAdresses"].setByFile("./Config/Config_FileAddresses.conf");
    int setFileCounter = config["Config_FileAdresses"]["DefaultSet"]["ConfigurationDataFileCount"][0].as<float>();
    int getFileCounter = setFileCounter;
    for (int fileCounter = 1; fileCounter <= getFileCounter; fileCounter++)
    {
        fileCounterValue = Str(fileCounter).as<string > ();
        configTopic = config["Config_FileAdresses"]["ConfigurationDataFileName"][fileCounterValue][0].as<string > ();
        config.add(configTopic);
        string file = "./Config/" + configTopic + ".conf";
        config[configTopic].setByFile("./Config/" + configTopic + ".conf");
        WarnLoadingTrue(" Loading Config_FileAdresses File: " + file);
    }
}

void Agent::initializeLogger(EntityType type, int id)
{
    //    Logger logger;
    string agent;
    switch (type)
    {
    case ET_FIRE_BRIGADE:
        agent = "FireBrigade";
        break;
    case ET_FIRE_STATION:
        agent = "FireStation";
        break;
    case ET_AMBULANCE_TEAM:
        agent = "AmbulanceTeam";
        break;
    case ET_AMBULANCE_CENTER:
        agent = "AmbulanceCenter";
        break;
    case ET_POLICE_FORCE:
        agent = "PoliceForce";
        break;
    case ET_POLICE_OFFICE:
        agent = "PoliceOffice";
        break;
    default:
        agent = "Agent";
        break;
    }

    agent += "-";

    const Timer * mTimer = new OnlineTimer();
    string logDir = config["LoggerInitialize"]["Logger"]["Directory"].asString();
    if (*logDir.rbegin() != '/')
        logDir += '/';
    if (config["LoggerInitialize"]["Logger"]["LogMain"].asBool())
    {
        //		logger.add(world->selfIDString, new LogFile(logDir + agent + string(Str(id).as<string > ()) + ".log", mTimer/*wm.getTimer()*/));
        logger.add("Main", new LogFile(logDir + agent + string(Str(id).as<string > ()) + ".log", mTimer/*wm.getTimer()*/));
        mkdir(logDir.c_str(), 16877);
    }
    else
        //		logger.add(world->selfIDString, new LogDevNull());
        logger.add("Main", new LogDevNull());

    if (config["LoggerInitialize"]["Logger"]["Offline"].asBool() && !config["ServerInitialize"]["Server"]["OfflinePlaying"].asBool() && doOffline)
    {
        //        doOffline = true;
        logger.add("MainOff", new LogFile(logDir + "/Offline/" + agent + string(Str(id).as<string > ()) + ".log.offline", mTimer/*wm.getTimer()*/), false);
    }
    else
    {
        //        doOffline = false;
        logger.add("MainOff", new LogDevNull(), false);
    }
    //	delete mTimer;
}

string Agent::getOfflineFileDir(EntityType et, int id)
{
    string agent;
    string logDir = config["LoggerInitialize"]["Logger"]["Directory"].asString();
    agent = logDir + "/Offline/";
    switch (et)
    {
    case ET_FIRE_BRIGADE:
        agent += "FireBrigade";
        break;
    case ET_FIRE_STATION:
        agent += "FireStation";
        break;
    case ET_AMBULANCE_TEAM:
        agent += "AmbulanceTeam";
        break;
    case ET_AMBULANCE_CENTER:
        agent += "AmbulanceCenter";
        break;
    case ET_POLICE_FORCE:
        agent += "PoliceForce";
        break;
    case ET_POLICE_OFFICE:
        agent += "PoliceOffice";
        break;
    default:
        agent += "Agent";
        break;
    }
    agent += "-" + string(Str(id).as<string > ()) + ".log.offline";
    return agent;
}
// return value: 0=ok, -2=connection error, -3=no more agents

int Agent::initData(EntityType type)
{
    initializeConfig();
    return 0;
}

void Agent::setConnectionParameters(string ip, int port)
{
    if (ip.length() > 3)
    {
        config["ServerInitialize"]["Connection"]["HostName"].setString(ip);
    }
    if (port != 0)
    {
        config["ServerInitialize"]["Connection"]["Port"].setInt(port);
    }
}

int Agent::initConnection(EntityType type, int offlineId)
{
    if (offlineId == -1)
    {
        connection = new OnlineConnection(config["ServerInitialize"]["Connection"]["HostName"].asString(), config["ServerInitialize"]["Connection"]["Port"].asInt());
        doOffline = true;
    }
    else
    {
        doOffline = false;
        connection = new OfflineConnection(getOfflineFileDir(type, offlineId));
    }
    if (!connection->init())
    {
        cerr << "Connection Error;" << endl;
        this->id = -2;
        return -2;
    }
    Message connectionMessage;
    vector<byte> message;
    connectionMessage.setAKConnect(type, 1);
    message = connectionMessage.getMessage();
    connection->sendMessage(message);
    connection->getMessage(message);
    //	cerr << "i got message" << endl;
    world = new WorldModel(config);
    string analyzeResult = world->init(message, type);
    if (analyzeResult == "Receive KA_CONNECT_OK")
    {
        this->id = world->selfID;
        cerr << "I Receive OK " << id << endl;
        int tmp = this->id, tmp2 = 0;
        initializeLogger(type, this->id);
        if (doOffline)
        {
            LOGL(MainOff) << message.size();
            for (int i = 0; i < (int) message.size(); i++)
            {
                LOGL(MainOff) << message[i];
            }
        }
        while (tmp > 0)
        {
            world->selfIDString[tmp2++] = tmp % 10 + '0';
            tmp /= 10;
        }
        world->selfIDString[tmp2] = 0;
        reverse(world->selfIDString, world->selfIDString + tmp2);
        this->type = type;
        this->startupConnectTime = atoi(world->config[Encodings::getConfigType(CT_STARTUP_CONNECT_TIME)].c_str());
        return 0;
    }
    else
    {
        //		cerr << "Server Connect Error; Reason: " << analyzeResult << endl;
        this->id = -3;
        return -3;
    }
}

void Agent::run()
{
    // implementation in child classes
    cout << "Agent running with id = " << this->id << endl;
    LOG(Main, 1) << "started with id: " << this->id << endl;
    this->agentThinkTime = atoi(world->config[Encodings::getConfigType(CT_THINK_TIME)].c_str());
    this->ignoreCommandsUntil = atoi(world->config[Encodings::getConfigType(CT_IGNORE_UNTIL)].c_str());
    this->damagePerception = atoi(world->config[Encodings::getConfigType(CT_DAMAGE_PERCEPTION)].c_str());
    this->hpPerception = atoi(world->config[Encodings::getConfigType(CT_HP_PERCEPTION)].c_str());
    precomputation();
    sendAKAcknowledge();
    //	cerr << "reaches to while true." << endl;
    Message connectionMessage;
    vector<byte> message;
    //        for(map<string, string>::iterator it = world->config.begin(); it != world->config.end(); it++)
    //        {
    //            cout << it->first <<  " " << it->second << endl;
    //        }
    while (true)
    {
        message.clear();
        if (!connection->getMessage(message))
        {
            cout << "Agent with id " << id << " disconnected" << endl;
            connection->done();
            break;
        }
        if (doOffline)
        {
            LOGL(MainOff) << message.size();
            for (int i = 0; i < (int) message.size(); i++)
            {
                LOGL(MainOff) << message[i];
            }
        }
        if (config["LoggerInitialize"]["Logger"]["LogMain"].asBool())
            ((Timer*) (LOGL(Main).timer()))->resetCycleTime();
        LOG(Main, 1) << endl << "[" << world->getTime() << "]" << endl;
        string result = world->update(message);
        if (!self()->isBuilding())
        {
            worldGraph->update();
        }
        actBeforeRadar();
        if (message.size() > 0 && world->getTime() > ignoreCommandsUntil)
        {
            radar->subscribe();
            LOG(Main, 1) << "before share sense " << endl;
            radar->shareSenseWorldAndSendOrders();
            LOG(Main, 1) << "after share sense " << endl;
        }
        act();
    }
}

void Agent::sendAKAcknowledge()
{
    Message connectionMessage;
    vector<byte> message;
    connectionMessage.setAKACKnowledge(1, world->selfID);
    message = connectionMessage.getMessage();
    connection->sendMessage(message);
}

Agent::~Agent()
{
}

void Agent::actBeforeRadar()
{
}

void Agent::act()
{
}

RCRObject* Agent::self()
{
    return world->self;
}
