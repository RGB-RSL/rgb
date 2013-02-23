/*
 * worldModel.cpp
 *
 *  Created on: Jan 27, 2010
 *      Author: Mehran Akhavan & Navid Saleh
 */

#include "WorldModel.h"
#include "../Utilities/Geometry/Circle.h"
#include "../Utilities/Geometry/Segment.h"
#include "../Utilities/Geometry/GeometryFunctions.h"
#include "../Utilities/DoubleUtilities.h"
#include "Config.h"
#include <fstream>
#include <map>
#include <algorithm>

#define LOGLEVEL 3

using namespace std;
using namespace Types;
using namespace Geometry;

WorldModel::WorldModel(Config &config) : teamConfig(config)
{
    self = new RCRObject(0);
    this->m_time = 0;
    world = this;
    //	self(0);
    //	policeOffice = PoliceOffice(0);
    //	fireStation = FireStation(0);
    //	ambulanceCenter =AmbulanceCenter(0);
}

bool RCRObjectCompare(RCRObject* o1, RCRObject* o2)
{
    return o1->getId() < o2->getId();
}

string WorldModel::init(vector<byte> message, EntityType type)
{
    string result = Parser::analyzeMessage(world, message);
    complete(type);
    return result;
}
//the complete funtion set things which didn't set in parser

void WorldModel::complete(EntityType type)
{
    if (type == ET_FIRE_BRIGADE)
    {
        for (int i = 0; i < (int) fireBrigades.size(); i++)
            if (this->selfID == (int) fireBrigades[i]->getId())
                self = fireBrigades[i];
    }
    else if (type == ET_FIRE_STATION)
    {
        for (int i = 0; i < (int) fireStations.size(); i++)
            if (this->selfID == (int) fireStations[i]->getId())
                self = fireStations[i];
    }
    else if (type == ET_AMBULANCE_TEAM)
    {
        for (int i = 0; i < (int) ambulanceTeams.size(); i++)
            if (this->selfID == (int) ambulanceTeams[i]->getId())
                self = ambulanceTeams[i];
    }
    else if (type == ET_AMBULANCE_CENTER)
    {
        for (int i = 0; i < (int) ambulanceCenters.size(); i++)
            if (this->selfID == (int) ambulanceCenters[i]->getId())
                self = ambulanceCenters[i];
    }
    else if (type == ET_POLICE_FORCE)
    {
        for (int i = 0; i < (int) policeForces.size(); i++)
            if (this->selfID == (int) policeForces[i]->getId())
                self = policeForces[i];
    }
    else if (type == ET_POLICE_OFFICE)
    {
        for (int i = 0; i < (int) policeOffices.size(); i++)
            if (this->selfID == (int) policeOffices[i]->getId())
                self = policeOffices[i];
    }
    sort(ambulanceCenters.begin(), ambulanceCenters.end(), RCRObjectCompare);
    for (int i = 0; i < (int) ambulanceCenters.size(); i++)
        ambulanceCenters[i]->ambulanceCenterIndex = i;
    sort(ambulanceTeams.begin(), ambulanceTeams.end(), RCRObjectCompare);
    for (int i = 0; i < (int) ambulanceTeams.size(); i++)
        ambulanceTeams[i]->ambulanceTeamIndex = i;
    sort(fireBrigades.begin(), fireBrigades.end(), RCRObjectCompare);
    for (int i = 0; i < (int) fireBrigades.size(); i++)
        fireBrigades[i]->fireBrigadeIndex = i;
    sort(fireStations.begin(), fireStations.end(), RCRObjectCompare);
    for (int i = 0; i < (int) fireStations.size(); i++)
        fireStations[i]->fireStationIndex = i;
    sort(policeForces.begin(), policeForces.end(), RCRObjectCompare);
    for (int i = 0; i < (int) policeForces.size(); i++)
        policeForces[i]->policeForceIndex = i;
    sort(policeOffices.begin(), policeOffices.end(), RCRObjectCompare);
    for (int i = 0; i < (int) policeOffices.size(); i++)
        policeOffices[i]->policeOfficeIndex = i;
    sort(humans.begin(), humans.end(), RCRObjectCompare);
    for (int i = 0; i < (int) humans.size(); i++)
        humans[i]->humanIndex = i;
    sort(platoons.begin(), platoons.end(), RCRObjectCompare);
    for (int i = 0; i < (int) platoons.size(); i++)
        platoons[i]->platoonIndex = i;
    sort(buildings.begin(), buildings.end(), RCRObjectCompare);
    for (int i = 0; i < (int) buildings.size(); i++)
        buildings[i]->buildingIndex = i;
    sort(roads.begin(), roads.end(), RCRObjectCompare);
    for (int i = 0; i < (int) roads.size(); i++)
        roads[i]->roadIndex = i;
    sort(refuges.begin(), refuges.end(), RCRObjectCompare);
    for (int i = 0; i < (int) refuges.size(); i++)
        refuges[i]->refugeIndex = i;
    sort(motionlessObjects.begin(), motionlessObjects.end(), RCRObjectCompare);
    for (int i = 0; i < (int) motionlessObjects.size(); i++)
        motionlessObjects[i]->motionlessIndex = i;
    sort(humans.begin(), humans.end(), RCRObjectCompare);
    for (int i = 0; i < (int) humans.size(); i++)
        humans[i]->humanIndex = i;
    //        cout << "number of mlos: " << motionlessObjects.size() << endl;
    for (int i = 0; i < (int) motionlessObjects.size(); i++)
    {
        //            cout << i << ' ' << motionlessObjects[i]->getId() << endl;
        for (int j = 0; j < (int) motionlessObjects[i]->getIDs().size(); j++)
        {
            //                cout << motionlessObjects[i]->getIDs()[j] << endl;
            if (motionlessObjects[i]->getIDs()[j] != 0)
            {
                MotionlessObject *tmp = (MotionlessObject*) objects[motionlessObjects[i]->getIDs()[j]];
                motionlessObjects[i]->setID(j, tmp->motionlessIndex);
                motionlessObjects[i]->addNeighbour(tmp->motionlessIndex);
            }
            else
            {
                motionlessObjects[i]->setID(j, -1);
            }
        }
        //            cout << "reached" << endl;
    }
    updatePositions();
    generateCloserThan();
    maxSightRange = atoi(config["perception.los.max-distance"].c_str());
    rayCount = (config.find("perception.los.ray-count") != config.end()) ? atoi(config["perception.los.ray-count"].c_str()) : 72;
}

string WorldModel::update(vector<byte> msg)
{
    string result = Parser::analyzeMessage(world, msg);
    sort(civilians.begin(), civilians.end(), RCRObjectCompare);
    for (int i = 0; i < (int) civilians.size(); i++)
        civilians[i]->civilianIndex = i;
    updateDeadAgents();
    updatePositions();
    setLastCylceUpdates();
    updateBlockades();
    setHasBeenSeens();
    setBuildingsInFire();
    updateHealthsAndDamages();
    updateInnerHumans();
    updatePassingModesAndRepresentiveNodes();
    return result;
}

void WorldModel::setWorldGraph(WorldGraph *worldGraph)
{
    this->worldGraph = worldGraph;
}

void WorldModel::setLastCylceUpdates()
{
    for (int i = 0; i < civilians.size(); i++)
    {
        if (civilians[i]->getLastCycleUpdated() == world->getTime())
            civilians[i]->getMotionlessObject()->setLastCycleUpdated(world->getTime());
    }
}

void WorldModel::updatePositions()
{
    for (int i = 0; i < (int) ambulanceTeams.size(); i++)
    {
        Human *human = ambulanceTeams[i];
        if (human->getMotionlessObject() == NULL || human->getMotionlessObject()->getId() != human->getPosition())
        {
            human->setMotionlessObject((MotionlessObject*) objects[human->getPosition()]);
            LOG(Main, 1) << "update position of " << human->getId() << " : " << human->getMotionlessObject()->getId() << endl;
        }
    }
    for (int i = 0; i < (int) humans.size(); i++)
    {
        Human *human = humans[i];
        if (!human->isAmbulanceTeam())
        {
            bool flag = false;
            for (int j = 0; j < (int) ambulanceTeams.size(); j++)
                if (ambulanceTeams[j]->getId() == human->getPosition())
                {
                    human->setMotionlessObject((MotionlessObject*) objects[ambulanceTeams[j]->getPosition()]);
                    LOG(Main, 1) << "update position of (loaded in ambulance) " << human->getId() << " : " << human->getMotionlessObject()->getId() << endl;
                    flag = true;
                    break;
                }
            if (human->getMotionlessObject() == NULL || (human->getMotionlessObject()->getId() != human->getPosition() && !flag))
            {
                human->setMotionlessObject((MotionlessObject*) objects[human->getPosition()]);
                LOG(Main, 1) << "update position of " << human->getId() << " : " << human->getMotionlessObject()->getId() << endl;
            }
        }
        if (human->getLastCycleUpdatedBySense() != world->getTime())
        {
            human->setX(human->getMotionlessObject()->getX());
            human->setY(human->getMotionlessObject()->getY());
        }
    }
    if (self->isPlatoon())
    {
        for (int i = 0; i < civilians.size(); i++)
        {
            if (((Platoon*) self)->getMotionlessObject()->motionlessIndex == civilians[i]->getMotionlessObject()->motionlessIndex && getTime() - civilians[i]->getLastCycleUpdated() > 1)
                civilians[i]->isAvailable = false;
        }
    }
    //set civilians to buildingBlocks
    if (self->isPoliceForce())
    {
        for (int i = 0; i < buildingBlocks.size(); i++)
            buildingBlocks[i]->civilians.clear();
        for (int i = 0; i < civilians.size(); i++)
        {
            if (civilians[i]->getMotionlessObject()->isBuilding())
            {
                LOG(Main, 4) << "civ: " << civilians[i]->getId() << endl;
                LOG(Main, 4) << "((Building*)civilians[i]->getMotionlessObject())->blockNum = " << ((Building*) civilians[i]->getMotionlessObject())->blockNum << endl;
                buildingBlocks[((Building*) civilians[i]->getMotionlessObject())->blockNum]->civilians.push_back(civilians[i]);
            }
        }
    }
    if (self->isPlatoon())
        ((Platoon*) self)->getMotionlessObject()->iHaveBeenHere = true;
}

void WorldModel::updateBlockades()
{
    for (set<Blockade *>::iterator blockade = blockades.begin(); blockade != blockades.end(); blockade++)
    {
        if ((*blockade)->getLastClearTime() == m_time - 1 && (*blockade)->getLastCycleUpdated() != m_time)
        {
            LOG(Main, 1) << "I cleared blockade: " << (*blockade)->getId() << endl;
            Road *r = (Road*) objects[(*blockade)->getPosition()];
            for (vector<Blockade*>::iterator i = r->getBlockades().begin(); i != r->getBlockades().end(); i++)
            {
                if ((*i)->getId() == (*blockade)->getId())
                {
                    r->getBlockades().erase(i);
                    break;
                }
            }
            objects[(*blockade)->getId()] = NULL;
            blockades.erase(blockade);
        }
    }
    for (int i = 0; i < roads.size(); i++)
    {
        if (roads[i]->getLastCycleUpdatedBySense() == m_time)
        {
            roads[i]->clearBlockades();
            for (int j = 0; j < (int) roads[i]->getBlockadeIds().size(); j++)
            {
                roads[i]->addBlockade((Blockade*) objects[roads[i]->getBlockadeIds()[j]]);
            }
        }
    }
}

void WorldModel::setHasBeenSeens()
{
    for (int i = 0; i < (int) platoons.size(); i++)
    {
        //		MotionlessObject *position = (MotionlessObject *)(objects[((Platoon *)(platoons[i]))->getPosition()]);
        MotionlessObject *position = platoons[i]->getMotionlessObject();
        //                LOG(Main, 1) << position << endl;
        position->lastTimeSeen = getTime();
        //		LOG(Main, 1) << "platoon with id " << platoons[i]->getId() << " saw building " << position->getId() << endl;
        //        for(int j = 0 ; j < motionlessObjects.size(); j++)
        //            if(motionlessObjects[j]->getPos().distFromPoint(self->getPos()) < MAX_SIGHT_RATE && position->neighbours.contains(position->neighbours, motionlessObjects[j]->getId()))
        //            {
        //                LOG(Main, 1) << "I saw building " << motionlessObjects[j]->getId() << " too" << endl;
        //                motionlessObjects[j]->lastTimeSeen = getTime();
        //            }
    }
}

void WorldModel::setBuildingsInFire()
{
    buildingsInFire.clear();
    for (int i = 0; i < (int) buildings.size(); i++)
        if (buildings[i]->getTemperature() >= 40 || buildings[i]->isBurning())
        {
            LOG(Main, 1) << "add building in fire id: " << buildings[i]->getId() << endl;
            LOG(Main, 1) << "temperature: " << buildings[i]->getTemperature() << endl;
            buildingsInFire.push_back(buildings[i]);
        }
}

void WorldModel::updateHealthsAndDamages()
{
    for (int i = 0; i < (int) civilians.size(); i++)
    {
        if (civilians[i]->getHp() <= 1 || civilians[i]->getMotionlessObject()->isRefuge())
        {
            civilians[i]->isAvailable = false;
            continue;
        }
        if (civilians[i]->getSpotedTime() == world->getTime())
        {
            civilians[i]->setEstimateHp(civilians[i]->getHp());
            civilians[i]->setDamage(civilians[i]->getDamage());
        }
        civilians[i]->setEstimateHp(civilians[i]->getEstimateHp() - 1);
        civilians[i]->setEstimateDamage(civilians[i]->getEstimateDamage());
        //		if(civilians[i]->getLastCycleUpdated() != getTime() && civilians[i]->isAvailable)
        //		{
        //			if(civilians[i]->getBuriedness() == 0)
        //			{
        //				estimateDamage++;
        //			}
        //			else
        //			{
        //				estimateDamage += civilians[i]->getBuriedness() * buriednessDamage;
        //			}
        //			if(civilians[i]->getMotionlessObject()->isBuilding())
        //			{
        //				if(((Building*)civilians[i]->getMotionlessObject())->getTemperature() > 40)
        //					estimateDamage += fireDamage;
        //			}
        //			estimateHp -= estimateDamage;
        //			civilians[i]->setEstimateDamage(estimateDamage);
        //			civilians[i]->setEstimateHp(estimateHp);
        //		}

    }
}

void WorldModel::setInitialDistanceFindingObjects()
{
    if (self->isPoliceForce())
    {
        for (int i = 0; i < (int) policeForces.size(); i++)
        {
            worldGraph->addDistanceFindingHumanIndex(policeForces[i]->humanIndex);
        }
        for (int i = 0; i < (int) refuges.size(); i++)
        {
            worldGraph->addDistanceFindingMotiolessIndex(refuges[i]->motionlessIndex);
        }
    }
    else if (self->isAmbulanceTeam())
    {
        for (int i = 0; i < (int) ambulanceTeams.size(); i++)
        {
            worldGraph->addDistanceFindingHumanIndex(ambulanceTeams[i]->humanIndex);
        }
        for (int i = 0; i < (int) refuges.size(); i++)
        {
            worldGraph->addDistanceFindingMotiolessIndex(refuges[i]->motionlessIndex);
        }
    }
    else if (self->isFireBrigade())
    {
        for (int i = 0; i < (int) fireBrigades.size(); i++)
        {
            worldGraph->addDistanceFindingHumanIndex(fireBrigades[i]->humanIndex);
        }
        for (int i = 0; i < (int) refuges.size(); i++)
        {
            worldGraph->addDistanceFindingMotiolessIndex(refuges[i]->motionlessIndex);
        }
    }
}

void WorldModel::updateDeadAgents()
{
    for (int i = 0; i < platoons.size(); i++)
        if (getTime() - platoons[i]->getLastCycleRecieveData() > 10 && platoons[i]->getId() != self->getId())
            platoons[i]->isAvailable = false;
        else
            platoons[i]->isAvailable = true;
}

void WorldModel::updateInnerHumans()
{
    for (int i = 0; i < (int) motionlessObjects.size(); i++)
    {
        motionlessObjects[i]->clearInnerHumans();
    }
    for (int i = 0; i < (int) humans.size(); i++)
    {
        if (humans[i]->getLastCycleUpdatedBySense() == m_time)
        {
            humans[i]->getMotionlessObject()->addInnerHumanIndex(i);
        }
    }
}

void WorldModel::updatePassingModesAndRepresentiveNodes()
{
    for (int i = 0; i < (int) humans.size(); i++)
    {
        if (humans[i]->getLastCycleUpdated() == m_time)
        {
            worldGraph->updateWithoutBlockadeRepresentiveNode(i);
        }
    }
    LOG(Main, 3) << "Update Passing Modes And Representive Nodes started." << endl;
    for (int i = 0; i < (int) motionlessObjects.size(); i++)
    {
        if (motionlessObjects[i]->getLastCycleUpdatedBySense() == m_time)
        {
            worldGraph->updatePassingModesAndRepresentiveNodes(i);
        }
    }
    LOG(Main, 3) << "Update Passing Modes And Representive Nodes started." << endl;
}

void WorldModel::setTime(int time)
{
    this->m_time = time;
}

WorldModel::~WorldModel()
{
}

int WorldModel::getTime()
{
    return m_time;
}

void WorldModel::generateCloserThan()
{
    for (int i = 0; i < motionlessObjects.size(); i++)
    {
        for (int j = 0; j < motionlessObjects.size(); j++)
        {
            //                  Motionless - Building
            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 30000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[j]->isBuilding())
            {

                motionlessObjects[i]->mCloserThan30B.push_back(motionlessObjects[j]);
            }
            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 40000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[j]->isBuilding())
            {

                motionlessObjects[i]->mCloserThan40B.push_back(motionlessObjects[j]);
            }
            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 50000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[j]->isBuilding())
            {

                motionlessObjects[i]->mCloserThan50B.push_back(motionlessObjects[j]);
            }
            //                  Building - Motionless
            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 30000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[i]->isBuilding())
            {
                Building * thisMotionlessBuilding = ((Building *) motionlessObjects[i]);
                thisMotionlessBuilding->closerThan30M.push_back(motionlessObjects[j]);
            }

            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 40000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[i]->isBuilding())
            {
                Building * thisMotionlessBuilding = ((Building *) motionlessObjects[i]);
                thisMotionlessBuilding->closerThan40M.push_back(motionlessObjects[j]);
            }

            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 50000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[i]->isBuilding())
            {
                Building * thisMotionlessBuilding = ((Building *) motionlessObjects[i]);
                thisMotionlessBuilding->closerThan50M.push_back(motionlessObjects[j]);
            }

            //                  Building - Road
            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 70000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[i]->isBuilding() &&
                    motionlessObjects[j]->isRoad())
            {
                Building * thisMotionlessBuilding = ((Building *) motionlessObjects[i]);
                Road * thisMotionlessRoad = ((Road *) motionlessObjects[j]);
                thisMotionlessBuilding->closerThan100R.push_back(thisMotionlessRoad);
            }

            //                  Building - Building
            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 20000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[i]->isBuilding() &&
                    motionlessObjects[j]->isBuilding())
            {
                Building * thisMotionlessBuilding1 = ((Building *) motionlessObjects[i]);
                Building * thisMotionlessBuilding2 = ((Building *) motionlessObjects[j]);
                thisMotionlessBuilding1->closerThan20B.push_back(thisMotionlessBuilding2);
            }
            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 30000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[i]->isBuilding() &&
                    motionlessObjects[j]->isBuilding())
            {
                Building * thisMotionlessBuilding1 = ((Building *) motionlessObjects[i]);
                Building * thisMotionlessBuilding2 = ((Building *) motionlessObjects[j]);
                thisMotionlessBuilding1->closerThan30B.push_back(thisMotionlessBuilding2);
            }
            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 40000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[i]->isBuilding() &&
                    motionlessObjects[j]->isBuilding())
            {
                Building * thisMotionlessBuilding1 = ((Building *) motionlessObjects[i]);
                Building * thisMotionlessBuilding2 = ((Building *) motionlessObjects[j]);
                thisMotionlessBuilding1->closerThan40B.push_back(thisMotionlessBuilding2);
            }
            if ((int) motionlessObjects[i]->getPos().distFromPoint(motionlessObjects[j]->getPos()) < 50000 &&
                    motionlessObjects[i]->getId() != motionlessObjects[j]->getId() &&
                    motionlessObjects[i]->isBuilding() &&
                    motionlessObjects[j]->isBuilding())
            {
                Building * thisMotionlessBuilding1 = ((Building *) motionlessObjects[i]);
                Building * thisMotionlessBuilding2 = ((Building *) motionlessObjects[j]);
                thisMotionlessBuilding1->closerThan50B.push_back(thisMotionlessBuilding2);
            }
        }
    }
}

void WorldModel::setRoadsInnerPoints()
{
    for (int i = 0; i < world->roads.size(); i++)
    {
        world->roads[i]->addInnerPoint(world->roads[i]->getPos());
    }
}

void WorldModel::setBuildingsInSightPoints()
{
    int extendedSightRange = maxSightRange * 2;
    LOG(Main, 2) << "num of roads: " << world->roads.size() << endl;
    for (int i = 0; i < (int) world->roads.size(); i++)
    {
        for (int j = 0; j < (int) world->roads[i]->getInnerPoints().size(); j++)
        {
            Point p = world->roads[i]->getInnerPoints()[j];
            vector<Building*> nearBuildings;
            //			vector<Point> nearPoints;
            //			vector<Point> upNearPoints;
            //			vector<Point> downNearPoints;
            LOG(Main, 2) << "road: " << i << " with id " << world->roads[i]->getId() << endl;
            for (int k = 0; k < (int) world->buildings.size(); k++)
            {
                if (distanceBetweenPoints(p, world->buildings[k]->getPos()) < extendedSightRange)
                {
                    nearBuildings.push_back(world->buildings[k]);
                    //					LOG(Main, 1) << "circle" << world->buildings[k]->getPos() << " 2000; ";
                }
            }
            //algorithm for sort near points
            //			for(int k = 0; k < (int)nearBuildings.size(); k++)
            //			{
            //				for(int l = 0; l < nearBuildings[k]->getShape().size(); l++)
            //				{
            //					Point tmp = nearBuildings[k]->getShape().getVertex(l);
            //					if(isLessEqual(p.getY(), tmp.getY()))
            //					{
            //						upNearPoints.push_back(tmp);
            //					}
            //					else
            //					{
            //						downNearPoints.push_back(tmp);
            //					}
            //				}
            //			}
            //			setComparePoint(p);
            //			sort(upNearPoints.begin(), upNearPoints.end(), Geometry::angleCompare);
            //			sort(downNearPoints.begin(), downNearPoints.end(), Geometry::angleCompare);
            //			for(int k = 0; k < (int)upNearPoints.size(); k++)
            //			{
            //				nearPoints.push_back(upNearPoints[k]);
            //			}
            //			for(int k = 0; k < (int)downNearPoints.size(); k++)
            //			{
            //				nearPoints.push_back(downNearPoints[k]);
            //			}
            //			LOG(Main, 2) << "after sort" << endl;
            //			if(nearPoints.size() == 0)
            //			{
            //				continue;
            //			}
            set<Building*> inSightBuildings;
            setColor(4);
            LOG(Main, 2) << Circle(p, maxSightRange) << ' ';
            LOG(Main, 2) << Circle(p, 500) << ' ';
            setColor(1);
            //			LOG(Main, 1) << "circle" << p << " " << extendedSightRange << "; ";
            //			for(int k = 1; k < (int)nearPoints.size(); k++)
            //			{
            //				LOG(Main, 1) << "line" << nearPoints[k - 1] << nearPoints[k] << "; ";
            //			}
            //			LOG(Main, 1) << "line" << nearPoints[0] << nearPoints.back() << "; ";
            //			for(int k = 0; k < (int)nearPoints.size(); k++)
            double dAngle = M_PI * 2 / rayCount;
            for (int k = 0; k < rayCount; k++)
            {
                //				int next = (k + 1) % nearPoints.size();
                //				Point mid = Segment(nearPoints[k], nearPoints[next]).getMiddlePoint();
                //				Segment s(p, mid);
                //				Vector ext = p.asVector() + s.asVector() * (extendedSightRange / s.getLength());
                //				s = Segment(p, ext.asPoint());
                double angle = k * dAngle;
                Segment s(p, (p.asVector() + Vector(sin(angle) * maxSightRange, cos(angle) * maxSightRange)).asPoint());
                LOG(Main, 2) << s << ' ';
                //				LOG(Main, 1) << "circle" << nearPoints[k] << " 200; ";
                double minDist = 1e10;
                Building* inSightBuilding = NULL;
                Segment minLine;
                for (int l = 0; l < (int) nearBuildings.size(); l++)
                {
                    for (int m = 0; m < nearBuildings[l]->getIDs().size(); m++)
                    {
                        if (nearBuildings[l]->getIDs()[m] == -1)
                        {
                            Segment edge = nearBuildings[l]->getShape().getSegment(m);
                            //							LOG(Main, 1) << "line" << edge.getFirstPoint() << edge.getSecondPoint() << "; ";
                            if (isIntersect(s, edge))
                            {
                                //								LOG(Main, 1) << "circle" << p << " " << maxSightRange << "; ";
                                //								LOG(Main, 1) << "circle" << p << " " << extendedSightRange << "; ";
                                //								LOG(Main, 1) << "line" << s.getFirstPoint() << s.getSecondPoint() << "; ";
                                //								LOG(Main, 1) << "line" << edge.getFirstPoint() << edge.getSecondPoint() << "; ";
                                Point intersect = getIntersectPoint(s.asLine(), edge.asLine());
                                //								LOG(Main, 1) << "circle" << intersect << " 200;" << endl;
                                if (isLess(distanceBetweenPoints(p, intersect), minDist))
                                {
                                    minDist = distanceBetweenPoints(p, intersect);
                                    inSightBuilding = nearBuildings[l];
                                    minLine = edge;
                                }
                            }
                        }
                    }
                }
                if (inSightBuilding != NULL && isLess(minDist, (double) maxSightRange))
                {
                    inSightBuildings.insert(inSightBuilding);
                    LOG(Main, 2) << Circle(inSightBuilding->getPos(), 1000) << ' ';
                    LOG(Main, 2) << minLine << ' ';
                    //					LOG(Main, 1) << "line" << s.getFirstPoint() << s.getSecondPoint() << "; ";
                }
            }
            //			LOG(Main, 2) << endl << endl;
            for (set<Building*>::iterator k = inSightBuildings.begin(); k != inSightBuildings.end(); k++)
            {
                (*k)->addInSightPoint(i, j);
            }
        }
        LOG(Main, 2) << endl;
    }
    // Reverse Log (buildings)
    //	for(int i = 0; i < (int)world->buildings.size(); i++)
    //	{
    //		LOG(Main, 2) << "building " << i << " with id " << world->buildings[i]->getId() << endl;
    //		setColor(0);
    //		LOG(Main, 2) << Circle(world->buildings[i]->getPos(), 1000) << ' ';
    //		setColor(2);
    //		for(int j = 0; j < (int)world->buildings[i]->getInSightPoints().size(); j++)
    //		{
    //			LOG(Main, 2) << Circle(world->roads[world->buildings[i]->getInSightPoints()[j].roadIndex]->getInnerPoints()[world->buildings[i]->getInSightPoints()[j].index], 500) << ' ';
    //		}
    //		LOG(Main, 2) << endl << endl;
    //	}
}