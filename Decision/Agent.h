/*
 * Agent.h
 *
 *  Created on: Dec 17, 2009
 *      Author: Mehran Akhavan
 */

#ifndef AGENT_H_
#define AGENT_H_

#include <tr1/memory>
#include <sys/stat.h>

#include "../WorldModel/WorldModel.h"
#include "../Connection/OnlineConnection.h"
#include <common.h>
#include <algorithm>

#include "../Utilities/Logger.h"
#include "../Utilities/Config.h"
#include "../Utilities/Debugger.h"
#include "../Connection/Message.h"
#include "../WorldGraph/WorldGraph.h"
#include "Command.h"
#include "../Radar/Radar.h"
#include "Search.h"

class Agent
{
public:
    Agent();
    virtual ~Agent();
    void run();
    int id;
    Types::EntityType type;
    int initData(Types::EntityType);
    void setConnectionParameters(std::string ip, int port);
    int initConnection(Types::EntityType, int);
protected:
    virtual void actBeforeRadar();
    virtual void act();
    //	virtual void initOfAgents();
    virtual void precomputation();
    Logger logger;
    virtual RCRObject* self();
    WorldModel *world;
    WorldGraph *worldGraph;
    Config config;
    AbstractConnection *connection;
    Command *command;
    Radar *radar;
    Search *search;
    int agentThinkTime;
    int ignoreCommandsUntil;
    int startupConnectTime;
    int damagePerception;
    int hpPerception;
private:
    void initializeLogger(Types::EntityType et, int id);
    void initializeConfig();
    void sendAKAcknowledge();
    std::string getOfflineFileDir(Types::EntityType, int);
    std::ofstream offline;
    bool doOffline;
};

#endif /* AGENT_H_ */