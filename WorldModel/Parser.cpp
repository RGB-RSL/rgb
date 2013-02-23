/*
 * Parser.cpp
 *
 *  Created on: Feb 22, 2010
 *      Author: Mehran Akhavan
 */

#include "../Utilities/Types/common.h"
#include "../Utilities/Types/input.h"
#include "../Utilities/Types/output.h"
#include "../Connection/OnlineConnection.h"
#include "Objects/Building.h"
#include <vector>
#include <cstring>
#include <iostream>
#include <cmath>
#include <netinet/in.h>
#include "Parser.h"
#include "../Utilities/Encodings.h"
#include "Objects/Blockade.h"
#include "../WorldGraph/WorldGraph.h"
#include "../Radar/Radar.h"
#define LOGLEVEL 2

using namespace std;
using namespace Librescue;
using namespace Types;
using namespace Geometry;

string Parser::analyzeMessage(WorldModel* world, vector<byte> &msg)
{
	int offset = 0;
	//	unsigned int size = msg.size();
	////cout << "size of whole message: " << size << endl;
	string header = Encodings::readString(msg, offset);
	//////cout << "Header: " << header << endl;
	//    offset += header.size() + 4;
	if(header == Encodings::getMessageType(MT_KA_CONNECT_OK))
		return parseKAConnectOk(world, msg, offset);
	else if(header == Encodings::getMessageType(MT_KA_CONNECT_ERROR))
		return parseKAConnectError(world, msg, offset);
	else if(header == Encodings::getMessageType(MT_KA_SENSE))
		return parseKASense(world, msg, offset);
	else if(header == Encodings::getMessageType(MT_KA_HEAR))
		return parseKAHear(world, msg, offset);
	else
		return "Not a valid header";
}

string Parser::parseKAConnectOk(WorldModel* world, vector<byte> &msg, int &offset)
{
	unsigned int size = Encodings::readInt(msg, offset);
	unsigned int tempId = Encodings::readInt(msg, offset);
	unsigned int realId = Encodings::readInt(msg, offset);
	world->selfID = realId;
	////cout << "my ID: " << realId << endl;
	parseObjectsElementsKAConnectOk(world, msg, offset);
	parseConfig(world, msg, offset);
	return "Receive KA_CONNECT_OK";
}

string Parser::parseKAConnectError(WorldModel* world, vector<byte> &msg, int &offset)
{
	int size = Encodings::readInt(msg, offset);
	int tempId = Encodings::readInt(msg, offset);
	string reason = Encodings::readString(msg, offset);
	return reason;
	//string reason of error

}

string Parser::parseKASense(WorldModel* world, vector<byte> &msg, int &offset)
{
	int size = Encodings::readInt(msg, offset);
	int id = Encodings::readInt(msg, offset);
	////cout << "id : " << id << endl;
	int time = Encodings::readInt(msg, offset);
	////cout << "time : " << time << endl;
	world->setTime(time);
	parseObjectsElementsKASense(world, msg, offset);
	int zero = Encodings::readInt(msg, offset);
	LOG(Main, 1) << "msg.size - offset: " << msg.size() - offset << endl;
	//	if(msg.size() - offset > 8)
	parseRadarMessages(world, msg, offset);
	return "Recive KA_SENSE";
}

string Parser::parseKAHear(WorldModel* world, vector<byte> &msg, int &strat)
{
	return "Recive KA_HEAR";
}
//this function read entity & id and then send it for parseObjectsElemens

void Parser::parseObjectsElementsKAConnectOk(WorldModel* world, vector<byte> &msg, int &offset)
{
	//numOfObjects is the numbe of objects
	int numOfObjects = Encodings::readInt(msg, offset);
	for(int i = 0; i < numOfObjects; i++)
	{
		int id;
		string entity;
		entity = Encodings::readString(msg, offset);
		////cout << "entity: " << entity << endl;
		id = Encodings::readInt(msg, offset);
		////cout << "id: " << id << endl;
		int len = Encodings::readInt(msg, offset);
		//counter is the number of properies
		unsigned int counter = Encodings::readInt(msg, offset);
		parseObjectElements(world, msg, offset, entity, id, counter);
	}
}

void Parser::parseObjectsElementsKASense(WorldModel* world, vector<byte>& msg, int& offset)
{
	//numOfObjects is the numbe of objects
	int numOfObjects = Encodings::readInt(msg, offset);
	for(int i = 0; i < numOfObjects; i++)
	{
		int id;
		string entity;
		id = Encodings::readInt(msg, offset);
		////cout << "id: " << id << endl;
		entity = Encodings::readString(msg, offset);
		////cout << "entity: " << entity << endl;
		//counter is the number of properies
		unsigned int counter = Encodings::readInt(msg, offset);
		parseObjectElements(world, msg, offset, entity, id, counter);
	}
}
//this function creat objects if thay don't exist and if it exist, it use the existed object and send it to parseObjects!!!!!!!!!

void Parser::parseObjectElements(WorldModel* world, vector<byte> &msg, int &offset, string type, int id, int counter)
{
	if(type == Encodings::getEntityType(ET_BULDING))
	{
		Building *b;
		if(world->objects[id] == NULL)
		{
			b = new Building(id);
			parseObjects(world, msg, *b, offset, counter);
			b->setLastCycleUpdated(world->getTime());
			b->setLastCycleUpdatedBySense(world->getTime());
			world->buildings.push_back(b);
			world->motionlessObjects.push_back(b);
			world->objects[id] = b;
		}
		else
		{
			b = (Building*)world->objects[id];
			b->setLastCycleUpdated(world->getTime());
			b->setLastCycleUpdatedBySense(world->getTime());
			parseObjects(world, msg, *b, offset, counter);
		}
	}
	else if(type == Encodings::getEntityType(ET_AMBULANCE_CENTER))
	{
		AmbulanceCenter *ac;
		if(world->objects[id] == NULL)
		{
			ac = new AmbulanceCenter(id);
			parseObjects(world, msg, *ac, offset, counter);
			ac->setLastCycleUpdated(world->getTime());
			ac->setLastCycleUpdatedBySense(world->getTime());
			world->ambulanceCenters.push_back(ac);
			world->motionlessObjects.push_back(ac);
			world->buildings.push_back(ac);
			world->objects[id] = ac;
		}
		else
		{
			ac = (AmbulanceCenter*)world->objects[id];
			ac->setLastCycleUpdated(world->getTime());
			ac->setLastCycleUpdatedBySense(world->getTime());
			parseObjects(world, msg, *ac, offset, counter);
		}
	}
	else if(type == Encodings::getEntityType(ET_AMBULANCE_TEAM))
	{
		AmbulanceTeam *at;
		if(world->objects[id] == NULL)
		{
			at = new AmbulanceTeam(id);
			parseObjects(world, msg, *at, offset, counter);
			at->setLastCycleUpdated(world->getTime());
			at->setLastCycleUpdatedBySense(world->getTime());
			world->ambulanceTeams.push_back(at);
			world->platoons.push_back(at);
			world->humans.push_back(at);
			world->objects[id] = at;
		}
		else
		{
			at = (AmbulanceTeam*)world->objects[id];
			at->setLastCycleUpdated(world->getTime());
			at->setLastCycleUpdatedBySense(world->getTime());
			parseObjects(world, msg, *at, offset, counter);
		}
	}
	else if(type == Encodings::getEntityType(ET_CIVILIAN))
	{
		Civilian *civ;
		if(world->objects[id] == NULL)
		{
			civ = new Civilian(id);
			parseObjects(world, msg, *civ, offset, counter);
			civ->setLastCycleUpdated(world->getTime());
			civ->setLastCycleUpdatedBySense(world->getTime());
			//			civ->setEstimateHp(civ->getHp());
			//			civ->setEstimateDamage(civ->getDamage());
			civ->setSpotedTime(world->getTime());
			world->civilians.push_back(civ);
			world->humans.push_back(civ);
			civ->humanIndex = world->humans.size() - 1;
			if(civ->getBuriedness() == 0)
				civ->isAvailable = false;
			world->objects[id] = civ;
		}
		else
		{
			civ = (Civilian*)world->objects[id];
			civ->setLastCycleUpdated(world->getTime());
			civ->setLastCycleUpdatedBySense(world->getTime());
			parseObjects(world, msg, *civ, offset, counter);
			civ->setEstimateHp(civ->getHp());
			civ->setEstimateDamage(civ->getDamage());
		}
	}
	else if(type == Encodings::getEntityType(ET_FIRE_BRIGADE))
	{
		FireBrigade *fb;
		if(world->objects[id] == NULL)
		{
			fb = new FireBrigade(id);
			parseObjects(world, msg, *fb, offset, counter);
			fb->setLastCycleUpdated(world->getTime());
			fb->setLastCycleUpdatedBySense(world->getTime());
			world->fireBrigades.push_back(fb);
			world->platoons.push_back(fb);
			world->humans.push_back(fb);
			world->objects[id] = fb;
		}
		else
		{
			fb = (FireBrigade*)world->objects[id];
			fb->setLastCycleUpdated(world->getTime());
			fb->setLastCycleUpdatedBySense(world->getTime());
			parseObjects(world, msg, *fb, offset, counter);
		}
	}
	else if(type == Encodings::getEntityType(ET_FIRE_STATION))
	{
		FireStation *fs;
		if(world->objects[id] == NULL)
		{
			fs = new FireStation(id);
			parseObjects(world, msg, *fs, offset, counter);
			fs->setLastCycleUpdated(world->getTime());
			fs->setLastCycleUpdatedBySense(world->getTime());
			world->fireStations.push_back(fs);
			world->motionlessObjects.push_back(fs);
			world->buildings.push_back(fs);
			world->objects[id] = fs;
		}
		else
		{
			fs = (FireStation*)world->objects[id];
			fs->setLastCycleUpdated(world->getTime());
			fs->setLastCycleUpdatedBySense(world->getTime());
			parseObjects(world, msg, *fs, offset, counter);
		}
	} //    case Librescue::TYPE_NODE:
		//    {
		//        Node *node;
		//        if (this->objects[id] == NULL)
		//        {
		//            node = new Node(id);
		//            parseObjects(msg, *node, offset);
		//            this->nodes.push_back(node);
		//            this->objects[id] = node;
		//        } else
		//        {
		//            node = (Node*)this->objects[id];
		//            parseObjects(msg, *node, offset);
		//        }
		//    }
	else if(type == Encodings::getEntityType(ET_POLICE_FORCE))
	{
		PoliceForce *pf;
		if(world->objects[id] == NULL)
		{
			pf = new PoliceForce(id);
			parseObjects(world, msg, *pf, offset, counter);
			pf->setLastCycleUpdated(world->getTime());
			pf->setLastCycleUpdatedBySense(world->getTime());
			world->policeForces.push_back(pf);
			world->platoons.push_back(pf);
			world->humans.push_back(pf);
			world->objects[id] = pf;
		}
		else
		{
			pf = (PoliceForce*)world->objects[id];
			pf->setLastCycleUpdated(world->getTime());
			pf->setLastCycleUpdatedBySense(world->getTime());
			parseObjects(world, msg, *pf, offset, counter);
		}
	}
	else if(type == Encodings::getEntityType(ET_POLICE_OFFICE))
	{
		PoliceOffice *po;
		if(world->objects[id] == NULL)
		{
			po = new PoliceOffice(id);
			parseObjects(world, msg, *po, offset, counter);
			po->setLastCycleUpdated(world->getTime());
			po->setLastCycleUpdatedBySense(world->getTime());
			world->policeOffices.push_back(po);
			world->motionlessObjects.push_back(po);
			world->buildings.push_back(po);
			world->objects[id] = po;
		}
		else
		{

			po = (PoliceOffice*)world->objects[id];
			po->setLastCycleUpdated(world->getTime());
			po->setLastCycleUpdatedBySense(world->getTime());
			parseObjects(world, msg, *po, offset, counter);
		}
	}
	else if(type == Encodings::getEntityType(ET_REFUGE))
	{
		Refuge *ref;
		if(world->objects[id] == NULL)
		{
			ref = new Refuge(id);
			parseObjects(world, msg, *ref, offset, counter);
			ref->setLastCycleUpdated(world->getTime());
			ref->setLastCycleUpdatedBySense(world->getTime());
			world->refuges.push_back(ref);
			world->motionlessObjects.push_back(ref);
			world->buildings.push_back(ref);
			world->objects[id] = ref;
		}
		else
		{
			ref = (Refuge*)world->objects[id];
			ref->setLastCycleUpdated(world->getTime());
			ref->setLastCycleUpdatedBySense(world->getTime());
			parseObjects(world, msg, *ref, offset, counter);
		}
	} //		case Librescue::TYPE_RIVER:
		//			////cout<<"id of river: " << id << endl;
		//			River *riv = new River(id);
		//			parseObjects(msg, *riv , offset);
		//			world->rivers.push_back(riv);
		//			break;
		//		case Librescue::TYPE_RIVER_NODE:
		//			////cout<<"id of river node: " << id << endl;
		//			RiverNode *rivNode = new RiverNode(id);
		//			parseObjects(msg, *rivNode , offset);
		//			world->riverNodes.push_back(rivNode);
		//			break;
	else if(type == Encodings::getEntityType(ET_ROAD))
	{
		Road *road;
		if(world->objects[id] == NULL)
		{
			road = new Road(id);
			parseObjects(world, msg, *road, offset, counter);
			road->setLastCycleUpdated(world->getTime());
			road->setLastCycleUpdatedBySense(world->getTime());
			world->roads.push_back(road);
			world->motionlessObjects.push_back(road);
			world->objects[id] = road;
		}
		else
		{
			road = (Road*)world->objects[id];
			road->setLastCycleUpdated(world->getTime());
			road->setLastCycleUpdatedBySense(world->getTime());
			road->clearBlockadeIds();
			parseObjects(world, msg, *road, offset, counter);
		}
	}
	else if(type == Encodings::getEntityType(ET_BLOCKADE))
	{
		Blockade *blockade;
		if(world->objects[id] == NULL)
		{
			blockade = new Blockade(id);
			parseObjects(world, msg, *blockade, offset, counter);
			blockade->setLastCycleUpdated(world->getTime());
			blockade->setLastCycleUpdatedBySense(world->getTime());
			world->blockades.insert(blockade);
			world->objects[id] = blockade;
		}
		else
		{
//			LOG(Main, 1) << "using existing blockade with id " << id << endl;
			blockade = (Blockade*)world->objects[id];
			blockade->clearShape();
			parseObjects(world, msg, *blockade, offset, counter);
			blockade->setLastCycleUpdated(world->getTime());
			blockade->setLastCycleUpdatedBySense(world->getTime());
		}
	}
	else
	{
		cerr << "unknown entity: " << type << endl;
		assert(false);
	}
}

void Parser::parseObjects(WorldModel* world, vector<byte> &msg, RCRObject &obj, int &offset, int counter)
{
	for(int i = 0; i < counter; i++)
	{
		string property = Encodings::readString(msg, offset);
		////cout << "property: " << property << endl;
		bool define = Encodings::readBool(msg, offset);
		if(define)
		{
			unsigned int lengthOfPropertyData = Encodings::readInt(msg, offset);
			if(lengthOfPropertyData > 4)
			{
				//counter2 is the number of property data
				int counter2 = Encodings::readInt(msg, offset);
				lengthOfPropertyData -= 4;
			}
			for(; lengthOfPropertyData > 0; lengthOfPropertyData -= 4)
			{
				unsigned int value = Encodings::readInt(msg, offset);
				obj.setProperties(property, value);
			}
		}
	}
}

void Parser::parseConfig(WorldModel* world, vector<byte>& msg, int& offset)
{
	int numOfConfigData = Encodings::readInt(msg, offset);
	for(int i = 0; i < numOfConfigData; i++)
	{
		string configType = Encodings::readString(msg, offset);
		string value = Encodings::readString(msg, offset);
		//		cout << "config: " << configType << endl;
		//		cout << "value: " << value << endl;
		world->config.insert(pair<string, string > (configType, value));
	}
}

void Parser::parseRadarMessages(WorldModel* world, vector<byte>& msg, int& offset)
{
	//counter is the number of messages
	int counter = Encodings::readInt(msg, offset);
	LOG(Main, 1) << "number of radar messages: " << counter << endl;
	for(int i = 0; i < counter; i++)
	{
		string speakHeader = Encodings::readString(msg, offset);
		int size = Encodings::readInt(msg, offset);
		LOG(Main, 1) << "size: " << size << endl;
		int sender = Encodings::readInt(msg, offset);
		LOG(Main, 1) << "sender: " << sender << endl;
		if(sender == world->selfID)
		{
			offset += size - 4;
			continue;
		}
		int time = Encodings::readInt(msg, offset);
		int channel = Encodings::readInt(msg, offset);
		LOG(Main, 1) << "channel: " << channel << endl;
		int sizeOfContent = Encodings::readInt(msg, offset);
		LOG(Main, 1) << "sizeOfContent: " << sizeOfContent << endl;
        if(world->objects[sender] != NULL && world->objects[sender]->isPlatoon())
            world->objects[sender]->setLastCycleRecieveData(world->getTime());
		world->radar->analyseMessage(msg, offset, sizeOfContent, sender, channel);
		offset += size - 16;
		//
	}
	LOG(Main, 1) << "radar ends" << endl;
}
