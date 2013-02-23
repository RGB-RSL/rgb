/*
 * worldModel.h
 *
 *  Created on: Jan 27, 2010
 *      Author: Alireza Kondori
 *      Modifier: Navid Saleh
 */

#ifndef WORLDMODEL_H_
#define WORLDMODEL_H_

#include "Objects/AmbulanceCenter.h"
#include "Objects/AmbulanceTeam.h"
#include "Objects/Building.h"
#include "Objects/Civilian.h"
#include "Objects/FireBrigade.h"
#include "Objects/FireStation.h"
#include "Objects/Human.h"
#include "Objects/Platoon.h"
#include "Objects/MotionlessObject.h"
#include "Objects/PoliceForce.h"
#include "Objects/PoliceOffice.h"
#include "Objects/RCRObject.h"
#include "Objects/Refuge.h"
#include "Objects/River.h"
#include "Objects/Road.h"
#include "Objects/BuildingsBlock.h"
#include "Objects/Blockade.h"
#include "../WorldGraph/WorldGraph.h"
#include "Parser.h"
#include "../Utilities/Geometry/Segment.h"
#include "../Utilities/Config.h"
//#include "../Utilities/logger.h"
#include <iostream>
#include <map>
class Radar;
class WorldGraph;

class WorldModel
{
public:
	WorldModel(Config&);
	std::string init(std::vector<byte> message, Types::EntityType type);
	virtual ~WorldModel();
	RCRObject *self;
	Radar* radar;
	std::vector<AmbulanceCenter *> ambulanceCenters;
	std::vector<PoliceOffice *> policeOffices;
	std::vector<FireStation *> fireStations;
	std::vector<Road *> roads;
	std::vector<Building *> buildings;
	std::vector<Building *> buildingsInFire;
	std::vector<Refuge *> refuges;
	std::vector<Human *> humans;
	std::vector<Platoon *> platoons;
	std::vector<Civilian *> civilians;
	std::vector<AmbulanceTeam *> ambulanceTeams;
	std::vector<FireBrigade *> fireBrigades;
	std::vector<PoliceForce *> policeForces;
	std::vector<River *> rivers;
	std::vector<MotionlessObject *> motionlessObjects;
	std::vector<BuildingBlock *> buildingBlocks;
        std::vector<BuildingBlock *> buildingBlocksInFire;
	std::set<Blockade *> blockades;
	std::map <int, RCRObject*> objects;
	//	std::vector<Nozzle> nozzles;
	std::string update(std::vector<byte> message);
	void updatePositions();
	void updateInnerHumans();
	void updatePassingModesAndRepresentiveNodes();
	void setInitialDistanceFindingObjects();
	void complete(Types::EntityType type);
	int getTime();
	void setTime(int time);
	void setWorldGraph(WorldGraph *worldGraph);
	void setRoadsInnerPoints();
	void setBuildingsInSightPoints();
	std::map<std::string, std::string> config;
	Config &teamConfig;
	int selfID;
	char selfIDString[10];
private:
	//		Logger logger;
	int m_time;
        void setLastCylceUpdates();
	void setHasBeenSeens();
	void updateBlockades();
	void setBuildingsInFire();
	void updateHealthsAndDamages();
	void generateCloserThan();
	void updateDeadAgents();
	int maxSightRange;
	int rayCount;
	WorldModel *world;
	WorldGraph *worldGraph;
};

#endif /* WORLDMODEL_H_ */
