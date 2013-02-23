#include "Agent.h"
#include "../Utilities/Geometry/Segment.h"

#define LOGLEVEL 1

const int Max_Distance_Neighbor = 100000;

using namespace std;
using namespace Types;

void Agent::precomputation()
{
	this->agentThinkTime = atoi(world->config[Encodings::getConfigType(CT_THINK_TIME)].c_str());
	this->ignoreCommandsUntil = atoi(world->config[Encodings::getConfigType(CT_IGNORE_UNTIL)].c_str());

	worldGraph = new WorldGraph(world);
	worldGraph->init();
	world->setWorldGraph(worldGraph);
	world->setInitialDistanceFindingObjects();
	for(int i = 0; i < (int)world->buildings.size(); i++)
	{
		for(int j = 0; j < (int)world->buildings[i]->getRelativeEdgesIndexes().size(); j++)
		{
			worldGraph->getEdges()[world->buildings[i]->getRelativeEdgesIndexes()[j]]->setPassingMode(PM_PASSABLE);
		}
	}
	world->updateInnerHumans();
	if(!self()->isBuilding())
	{
		worldGraph->update();
	}
	command = new Command(worldGraph, world, connection);
	radar = new Radar(world, worldGraph, command);
	world->radar = radar;
    search = new Search(world, worldGraph);
}