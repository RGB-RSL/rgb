/*
 * Command.cpp
 *
 *  Created on: Feb 8, 2010
 *      Author: Alimohammad
 *      Modifier: mehran
 */

#include "Command.h"
#include "../Connection/OnlineConnection.h"
#include "../Utilities/Debugger.h"
#include "../WorldGraph/WorldGraph.h"

#define LOGLEVEL 1

using namespace std;
using namespace Types;
using namespace Geometry;

Command::Command(WorldGraph* worldGraph, WorldModel* world, AbstractConnection *connection)
{
	this->worldGraph = worldGraph;
	this->world = world;
	this->connection = connection;
}

Command::~Command()
{
}

void Command::load(Human &target)
{
	load(target, world->getTime() + 1);
}

void Command::load(Human &target, int time)
{
    LOG(Main, 1) << "Load " << target.getId() << endl;
    ((Platoon*)world->self)->myLastCommand = CT_LOAD;
	((AmbulanceTeam*)world->self)->loadedHuman = &target;
	cm.setAKLoad(world->selfID, time, target.getId());
	message = cm.getMessage();
	connection->sendMessage(message);
}

void Command::unload()
{
	unload(world->getTime());
}

void Command::unload(int time)
{
    LOG(Main, 1) << "Unload" << endl;
    ((Platoon*)world->self)->myLastCommand = CT_UNLOAD;
	((AmbulanceTeam*)world->self)->loadedHuman = NULL;
	((AmbulanceTeam*)world->self)->unloadedTime = world->getTime();
	cm.setAKUnload(world->selfID, time);
	message = cm.getMessage();
	connection->sendMessage(message);
}

void Command::rescue(const Human &target)
{
	rescue(target, world->getTime());
}

void Command::rescue(const Human &target, int time)
{
    LOG(Main, 1) << "Rescue " << target.getId() << endl;
    ((Platoon*)world->self)->myLastCommand = CT_RESCUE;
	((AmbulanceTeam*)world->self)->lastRescueTime = world->getTime();
	cm.setAKRescue(world->selfID, time, target.getId());
	message = cm.getMessage();
	connection->sendMessage(message);
}

void Command::rest()
{
	rest(world->getTime());
}

void Command::rest(int time)
{
	LOG(Main, 1) << "Rest" << endl;
    ((Platoon*)world->self)->myLastCommand = CT_REST;
	cm.setAKRest(world->selfID, time);
	message = cm.getMessage();
	connection->sendMessage(message);
}

void Command::extinguish(const Building& target, int water)
{
	extinguish(target, water, world->getTime());
}

void Command::extinguish(const Building &target, int water, int time)
{
	LOG(Main, 1) << "Extinguish " << target.getId() << "Fiertyness " << target.getFieryness() << " temperture " << target.getTemperature() << endl;
    ((Platoon*)world->self)->myLastCommand = CT_EXTINGUISH;
	((FireBrigade*)world->self)->lastTimeExtinguish = world->getTime();
	cm.setAKExtinguish(world->selfID, time, target.getId(), water);
	message = cm.getMessage();
	connection->sendMessage(message);
}

void Command::clear(Blockade& target)
{
	clear(target, world->getTime());
}

void Command::clear(Blockade &target, int time)
{
	LOG(Main, 1) << "Clear " << target.getId() << endl;
    ((Platoon*)world->self)->myLastCommand = CT_CLEAR;
	((PoliceForce*)world->self)->lastTimeCleared = world->getTime();
	target.setLastClearTime(world->getTime());
	cm.setAKClear(world->selfID, time, target.getId());
	message = cm.getMessage();
	connection->sendMessage(message);
}

void Command::move(std::vector<int>& path)
{
	move(path, -1, -1, world->getTime());
}

void Command::move(std::vector<int>& path, int time)
{
	move(path, -1, -1, time);
}

void Command::move(std::vector<int>& path, int destinationX, int destinationY)
{
	move(path, destinationX, destinationY, world->getTime());
}

void Command::move(vector<int> &path, int destinationX, int destinationY, int time)
{
    LOG(Main, 1) << "Move ";
    for(int i = 0; i < path.size(); i++)
        LOG(Main, 1) << path[i] << " ";
    LOG(Main, 1) << "(" << destinationX << ", " << destinationY << ")" << endl;
    ((Platoon*)world->self)->myLastCommand = CT_MOVE;
	cm.setAKMove(world->selfID, time, path, destinationX, destinationY);
	message = cm.getMessage();
	connection->sendMessage(message);
}

void Command::speak(int channel, vector<byte> data)
{
	cm.setAKSpeak(world->selfID, world->getTime(), channel, data);
	message = cm.getMessage();
	connection->sendMessage(message);
}


void Command::subscribe(vector<int> channels)
{
	//	cout << "in subscribe" << endl;
	cm.setAKSubscribe(world->selfID, world->getTime(), channels);
	message = cm.getMessage();
	connection->sendMessage(message);
}

void Command::moveToMotionless(int motionlessIndex, GraphMode graphMode)
{

	vector<int> path;
	if(graphMode == GM_WITHOUT_BLOCKADE)
	{
		path = getPathToNode(world->motionlessObjects[motionlessIndex]->getWithoutBlockadeRepresentiveNodeIndex(), graphMode);
	}
	else
	{
		path = getPathToNode(world->motionlessObjects[motionlessIndex]->getRepresentiveNodeIndex(), graphMode);
	}
	if(path.size() == 0 || path.back() != world->motionlessObjects[motionlessIndex]->getId())
	{
		path.push_back(world->motionlessObjects[motionlessIndex]->getId());
	}
//	for(int i = 0; i < path.size(); i++)
//	{
//		LOG(Main, 1) << path[i] << ' ';
//	}
//	LOG(Main, 1) << endl;
	move(path);
}

void Command::moveToHuman(int humanIndex, Types::GraphMode graphMode)
{
	vector<int> path;
	if(graphMode == GM_WITHOUT_BLOCKADE)
	{
		if(world->humans[humanIndex]->getWithoutBlockadeRepresentiveNodeIndex() == -1)
		{
			LOG(Main, 1) << "Human " << humanIndex << "'s place is not known or it's not reachable." << endl;
			return;
		}
		path = getPathToNode(world->humans[humanIndex]->getWithoutBlockadeRepresentiveNodeIndex(), graphMode);
	}
	else
	{
		if(world->humans[humanIndex]->getRepresentiveNodeIndex() == -1)
		{
			LOG(Main, 1) << "Human " << humanIndex << "'s place is not known or it's not reachable." << endl;
			return;
		}
		path = getPathToNode(world->humans[humanIndex]->getRepresentiveNodeIndex(), graphMode);
	}
	if(path.size() == 0 || path.back() != world->humans[humanIndex]->getMotionlessObject()->getId())
	{
		path.push_back(world->humans[humanIndex]->getMotionlessObject()->getId());
	}
	move(path, (int)world->humans[humanIndex]->getPos().getX(), (int)world->humans[humanIndex]->getPos().getY());
}

void Command::moveToPoint(int motionlessIndex, Point p, Types::GraphMode graphMode)
{
	int repNode;
	if(graphMode == GM_WITHOUT_BLOCKADE)
	{
		//this can be better (but if it used a lot) (Eqbal)
		repNode = world->motionlessObjects[motionlessIndex]->getWithoutBlockadeRepresentiveNodeIndex();
	}
	else
	{
		repNode = worldGraph->getDynamicRepresentiveNode(motionlessIndex, p);
		if(repNode == -1)
		{
			LOG(Main, 1) << "Point(" << p.getX() << ", " << p.getY() << ") is not reachable." << endl;
			return;
		}
	}
	vector<int> path = getPathToNode(repNode, graphMode);
	if(path.size() == 0 || path.back() != world->motionlessObjects[motionlessIndex]->getId())
	{
		path.push_back(world->motionlessObjects[motionlessIndex]->getId());
	}
	move(path, (int)p.getX(), (int)p.getY());
}

vector<byte> Command::getMessage()
{
	return message;
}

vector<int> Command::getPathToNode(int nodeIndex, Types::GraphMode graphMode)
{
	vector<int> path;
//	LOG(Main, 1) << "getPath(" << ((Human*)world->self)->getRepresentiveNodeIndex() << ", " << nodeIndex << ")" << endl;
	if(graphMode == GM_WITHOUT_BLOCKADE)
	{
		path = worldGraph->getPath(((Human*)world->self)->getWithoutBlockadeRepresentiveNodeIndex(), nodeIndex, graphMode);
	}
	else
	{
		path = worldGraph->getPath(((Human*)world->self)->getRepresentiveNodeIndex(), nodeIndex, graphMode);
	}
//	for(int i = 0; i < path.size(); i++)
//	{
//		LOG(Main, 1) << path[i] << ' ';
//	}
//	LOG(Main, 1) << endl;
	if(path.size() > 0)
	{
		path = worldGraph->getIDPath(path);
		return path;
	}
	else
	{
		LOG(Main, 1) << "Node " << nodeIndex << " is not reachable." << endl;
		return path;
	}
}