/* 
 * File:   Search.cpp
 * Author: mehran
 * 
 * Created on January 19, 2011, 10:25 AM
 */

#include "Search.h"
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOGLEVEL 1
using namespace std;
using namespace Types;
Search::Search()
{
}

Search::Search(WorldModel* world, WorldGraph* worldGraph)
{
    this->world = world;
    this->worldGraph = worldGraph;
    myZone = NULL;
}

bool buildingBlockComparator(BuildingBlock* b1, BuildingBlock* b2)
{
    int value1 = MAX_INT, value2 = MAX_INT;
    for(int i = 0 ; i < b1->buildings.size(); i++)
        if(b1->buildings[i]->getX() + b1->buildings[i]->getY() < value1)
            value1 = b1->buildings[i]->getX() + b1->buildings[i]->getY();
    for(int i = 0 ; i < b2->buildings.size(); i++)
        if(b2->buildings[i]->getX() + b2->buildings[i]->getY() < value2)
            value2 = b2->buildings[i]->getX() + b2->buildings[i]->getY();
    return value1 < value2;
}

void Search::init(MotionlessObject* lastDuties[])
{
    creatZones();
    assignPoliceForcesToZones(lastDuties);
    LOG(Main, 1) << endl;
}

void Search::creatZones()
{
    vector<BuildingBlock*> buildingBlocks = world->buildingBlocks;
    sort(buildingBlocks.begin(), buildingBlocks.end(), buildingBlockComparator);
    int numberOfBuildingsForEachPolice = world->buildings.size() / world->policeForces.size();
    LOG(Main, 1) << "numberOfBuildingsForEachPolice = " << numberOfBuildingsForEachPolice << endl;

    int coordinator = 0;
    int j = 0;
    for(int i = 0; i < world->policeForces.size(); i++)
    {
        Zone* newZone = new Zone();
        int counter = 0;
        for( ;counter < numberOfBuildingsForEachPolice - coordinator && j < buildingBlocks.size(); j++)
        {
//            LOG(Main, 1) << "buildingBlock added " << buildingBlocks[j]->buildings[0]->getId() <<endl;
            newZone->buildingBlocks.push_back(buildingBlocks[j]);
            counter += buildingBlocks[j]->buildings.size();
//            LOG(Main, 1) << "counter= " << counter << endl;
        }
//        LOG(Main, 1) << "counter = " << counter << endl;
        coordinator = counter - numberOfBuildingsForEachPolice  + coordinator;
//        LOG(Main, 1) << "coordinator = " << coordinator << endl;
        zones.push_back(newZone);
    }
//    LOG(Main, 1) << "ajab" << endl;
    for(; j <buildingBlocks.size(); j++)
        zones[zones.size() - 1]->buildingBlocks.push_back(buildingBlocks[j]);
    int color = 0;
//    LOG(Main, 1) << "ajab2" << endl;
//    LOG(Main, 1) << "zones.size() " << zones.size() << endl;
    for(int i = 0 ; i < zones.size(); i++)
    {

//        LOG(Main, 1) << endl << "newZone " << i << endl;
//        LOG(Main, 1) << "buildingBlocks.size " << zones[i]->buildingBlocks.size()- 1 << endl;
        for(int j2 = 0 ; j2 < (int)zones[i]->buildingBlocks.size() - 1; j2++)
        {
            LOG(Main, 1) << "line" << zones[i]->buildingBlocks[j2]->buildings[0]->getPos() << zones[i]->buildingBlocks[j2 + 1]->buildings[0]->getPos() << " " << color << "; ";
        }
        if(color == 4)
            color = 0;
        else
            color++;
    }
    LOG(Main, 1) << endl;
}

void Search::assignPoliceForcesToZones(MotionlessObject* lastDuties[])
{
    set<int> busyPolices;
    eachPoliceZone.resize(world->policeForces.size());
    for(int i = 0 ; i < world->policeForces.size(); i++)
    {
        if(zones[i]->buildingBlocks.size() > 0)
        {
            int minDist = MAX_INT;
            PoliceForce* minPF = NULL;
            for(int j = 0 ; j < world->policeForces.size(); j++)
            {
                int dist = worldGraph->getDistance(lastDuties[world->policeForces[j]->policeForceIndex]->getWithoutBlockadeRepresentiveNodeIndex(), zones[i]->buildingBlocks[0]->getCenter()->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
            //            int dist = worldGraph->getDistance(world->policeForces[j]->getWithoutBlockadeRepresentiveNodeIndex(), zones[i]->buildingBlocks[0]->getCenter()->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
                if(dist < minDist && busyPolices.find(world->policeForces[j]->policeForceIndex) == busyPolices.end())
                {
                    minDist = dist;
                    minPF = world->policeForces[j];
                }
            }
            if(minPF != NULL)
            {
                zones[i]->policeForceIndex = minPF->policeForceIndex;
            //            LOG(Main, 1) << "police " << minPF->getId() << " matched to zone " << zones[i]->buildingBlocks[0]->getCenter()->getId() << endl;
                LOG(Main, 1) << "line" << minPF->getPos() << zones[i]->buildingBlocks[0]->getCenter()->getPos() << " 2; ";
                busyPolices.insert(minPF->policeForceIndex);
                eachPoliceZone[minPF->policeForceIndex] = i;
                if(minPF->policeForceIndex == ((PoliceForce*)world->self)->policeForceIndex)
                    myZone = zones[i];
            }
            else
            {
                LOGL(Main) << "WARNING, NOBODY MATCHES TO A ZONE IN Search.assignPoliceForcesToZones; THIS IS A BUG" << endl;
            }

        }
    }
}

Building* Search::actAmbulanceAndFireBrigadeSearch(bool canGoRefuge)
{
    LOG(Main, 1) << "actAmbulanceSearch begins" << endl;
    AmbulanceTeam* self = ((AmbulanceTeam*)world->self);
    vector<Building*> buildings;
    int limitedDist = 200000, counter = 0;
    while(counter < 4 && buildings.size() == 0)
    {
        for(int i = 0 ; i < world->buildings.size(); i++)
        {
            if(!canGoRefuge && world->buildings[i]->isBurning())
                continue;
    //        LOG(Main, 1) << "building " << world->buildings[i]->getId() << " isReachable: " << worldGraph->isReachable(self->getRepresentiveNodeIndex(), world->buildings[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED) << endl;
            if( worldGraph->getDistance(self->getRepresentiveNodeIndex(), world->buildings[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED) < limitedDist &&
                worldGraph->isReachable(self->getRepresentiveNodeIndex(), world->buildings[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED) )
                buildings.push_back(world->buildings[i]);
        }
        counter++;
        limitedDist += 100000;
    }
    if(buildings.size() > 0)
    {
        int random = rand() % buildings.size();
        return buildings[random];
    } else
        return NULL;
}

Building* Search::actPoliceSearch(bool canIGoRefuge)
{
    if(myZone == NULL)
        return NULL;
//    Zone* myZone;
    PoliceForce* self = (PoliceForce*)world->self;
    int minDist = MAX_INT;
    Building* target = NULL;
    for(int i = 0 ; i < myZone->buildingBlocks.size(); i++)
    {
        for(int j = 0 ; j < myZone->buildingBlocks[i]->buildings.size(); j++)
        {
            Building* b = myZone->buildingBlocks[i]->buildings[j];
            if(!canIGoRefuge && b->isBurning())
                continue;
//            LOG(Main, 2) << "b->id = " << b->getId() << endl;
            if(b->lastTimeSeen == -1)
            {
//                LOG(Main, 2) << "b wasn't seen before" << endl;
                int dist = worldGraph->getDistance(self->getWithoutBlockadeRepresentiveNodeIndex(), b->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
                if(dist < minDist && b->getTemperature() < 130)
                {
                    minDist = dist;
                    target = b;
                }
            }
        }
    }
    if(target != NULL)
        LOG(Main, 1) << "target for search = " << target->getId() << endl;
    else
        LOG(Main, 1) << "target for search is NULL " << endl;
    return target;
}

Building* Search::updateCiviliansAndZone(bool canIGoRefuge)
{
    if(myZone == NULL )
        return NULL;
    for(int i = 0 ; i < world->policeForces.size(); i++)
    {
        if(!world->policeForces[i]->isAvailable || world->policeForces[i]->getBuriedness() > 0)
        {
            LOG(Main, 1) << "1" << endl;
            Zone* z = zones[eachPoliceZone[world->policeForces[i]->policeForceIndex] ];
            int minDist = MAX_INT;
            Building* minBuilding = NULL;
            for(int j = 0 ; j < z->buildingBlocks.size(); j++ )
            {
                for(int k = 0 ; k < z->buildingBlocks[j]->buildings.size(); k++)
                {
                    Building* b = z->buildingBlocks[j]->buildings[k];
                    LOG(Main, 1) << "building b " << b->getId() << " " << b->lastTimeSeen << endl;
                    if(b->lastTimeSeen == -1)
                    {
                        int dist = worldGraph->getDistance(((PoliceForce*)world->self)->getRepresentiveNodeIndex(), b->getRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
                        if(dist < minDist)
                        {
                            minDist = dist;
                            minBuilding = b;
                        }
                    }
                }
            }
            if(minBuilding != NULL)
            {
                return minBuilding;
            }
        }
    }
    int minTime =0;
    for(int i = 0 ;i < myZone->buildingBlocks.size(); i++)
        minTime += myZone->buildingBlocks[i]->buildings.size();
    minTime /= 1.5;
    minTime += 10;
    LOG(Main, 1) << "min Time " << minTime << endl;
    Building* target = NULL;
    for(int i = 0; i < myZone->buildingBlocks.size(); i++)
    {
        for(int j = 0; j < myZone->buildingBlocks[i]->civilians.size(); j++)
        {
            Civilian* civ = myZone->buildingBlocks[i]->civilians[j];
            LOG(Main, 1) << "world->getTime() - civ->getLastCycleUpdated(): " << world->getTime() - civ->getLastCycleUpdated() << endl;
            if(world->getTime() - civ->getLastCycleUpdated() > minTime && civ->isAvailable && !civ->getMotionlessObject()->isRefuge())
            {
                LOG(Main, 1) << "civ " << civ->getId() << " wasn't update for a long time!" << endl;
                target = (Building*)civ->getMotionlessObject();
                break;
            }
        }
    }
    if(target == NULL)
    {
        int counter = 0;
        int limit = 5;
        while(target == NULL && counter < limit)
        {
            srand(time(NULL));
            int rand1 = rand() % myZone->buildingBlocks.size();
            int rand2 = rand() % myZone->buildingBlocks[rand1]->buildings.size();
            target = myZone->buildingBlocks[rand1]->buildings[rand2];
            if(!canIGoRefuge && target->isBurning())
            {
                target = NULL;
                continue;
            }
            LOG(Main, 2) << "I randomly choose " << target->getId() << endl;
            counter++;
        }
    }
    return target;
}

Building* Search::searchNearBuilding()
{
    vector<Building*> buildings(world->buildings);
    int minValue = MAX_INT, minIndex = 0;
    srand(time(NULL));
    for(int i = 0; i < buildings.size(); i++)
    {
//        if(!world->self->isPoliceForce() && !worldGraph->isReachable(((Platoon*)world->self)->getRepresentiveNodeIndex(), buildings[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
//            continue;
        LOG(Main, 1) << "before getting distance" << endl;
        int value = buildings[i]->lastTimeSeen * 20000 + worldGraph->getDistance(((Platoon*)world->self)->getWithoutBlockadeRepresentiveNodeIndex(), buildings[i]->getRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE) + rand() % 50000;
        LOG(Main, 1) << "last time seen of " << buildings[i]->getId() <<  " is " << buildings[i]->lastTimeSeen << endl;
        if(value < minValue)
        {
            minValue = value;
            minIndex = i;
        }
    }
    return buildings[minIndex];
}
Search::~Search()
{
}

