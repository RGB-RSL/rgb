/* 
 * File:   FireBrigadeAgent.cpp
 * Author: mehran
 * 
 * Created on September 24, 2010, 3:57 PM
 */

#include "FireBrigadeAgent.h"
#include "../Utilities/Debugger.h"
#include "Types.h"
#include "../Utilities/Geometry/GeometryFunctions.h"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <vector>
#include "../Utilities/Debugger.h"
#include "AmbulanceTeamAgent.h"
#include "RCRObject.h"
#include "Building.h"
#include "Human.h"
#include "FireBrigade.h"
#include "BuildingsBlock.h"
#include "Road.h"
#include "Command.h"
#include "Circle.h"
#define LOGLEVEL 1
#define MAX_DISTANCE 50000
#define MAX_BETWEEN_TWO_AGENT 100000
#define BUILDINGBLOCK_NEED_FIREBRIGADES 1000
#define VISION_RADIUS 30000
#define MAX_TEMPERATURE 40


using namespace std;
using namespace Types;
using namespace Geometry;

bool bbComparator(BuildingBlock* bb1, BuildingBlock* bb2)
{
    return bb1->value > bb2->value;
}

bool bCompare(Building * i, Building * j)
{
    return i->getValue() > j->getValue();
}

FireBrigadeAgent::FireBrigadeAgent()
{
}

FireBrigadeAgent::~FireBrigadeAgent()
{
}

void FireBrigadeAgent::precomputation()
{
    maxExtinguishDistance = atoi(world->config["fire.extinguish.max-distance"].c_str());
    maxFireTank = atoi(world->config["fire.tank.maximum"].c_str());
    maxWaterForExtinguish = atoi(world->config["fire.extinguish.max-sum"].c_str());
    maxSightRange = atoi(world->config["perception.los.max-distance"].c_str());
    Agent::precomputation();
    LOG(Main, 1) << "CanSee started" << endl;
    world->setRoadsInnerPoints();
    world->setBuildingsInSightPoints();
    LOG(Main, 1) << "CanSee finished" << endl;
    myBuildingBlock = NULL;
    myBuilding = NULL;
    dontShas[0] = dontShas[1] = dontShas[2] = dontShas[3] = false;
    lastMotionless = NULL;
    isNearMatching = false;
    createBuildingBlock();
    updateBuildingBlocks();

}

void FireBrigadeAgent::actBeforeRadar()
{
}

void FireBrigadeAgent::act()
{
    LOG(Main, 1) << "I am firebrigade" << endl;
    if (world->getTime() < ignoreCommandsUntil)
    {
        return;
    }

    //	{
    //		Edge *edge = worldGraph->getEdges()[i];
    //		switch(edge->getPassingMode())
    //		{
    //		case PM_PASSABLE:
    //			setColor(0);
    //			break;
    //		case PM_NOT_PASSABLE:
    //			setColor(4);
    //			break;
    //		case PM_UNKNOWN:
    //			setColor(1);
    //			break;
    //		}
    //		LOG(Main, 1) << Segment(worldGraph->getNodes()[edge->getFirstNodeIndex()]->getRepresentivePoint(), worldGraph->getNodes()[edge->getSecondNodeIndex()]->getRepresentivePoint()) << ' ';
    //	}
    //	LOG(Main, 1) << endl;

    //    command->moveToMotionless(world->refuges[0]->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
    LOG(Main, 1) << "my rep node " << self()->getRepresentiveNodeIndex() << endl;
    if (dontShasWhenShasingInAMotionless())
        return;
    if (dontShasWhenIAmNotReachable())
        return;
    canGoRefuge();
    //escape
    if (escape())
        return;
    if (self()->getMotionlessObject()->isRefuge())
        if (stayInRefuge())
        {
            command->rest();
            return;
        }
    if (needToGoToRefuge())
    {
        moveToRefuges();
        return;
    }
    LOG(Main, 1) << "after escape" << endl;
//    for (int i = 0; i < world->buildings.size(); i++)
//        if (world->buildings[i]->isBurning() || (world->buildings[i]->getTemperature() > 40 && world->buildings[i]->getFieryness() != 8))
//            LOG(Main, 1) << "Building in fire " << world->buildings[i]->getId() << endl;
    setBuildingBlocksInFire();
    eraseMyTargets();
    int actTime = 16 ;//+ buildingBlocksInFire.size();
    LOG(Main, 1) << "I want to decision every " << actTime << " and be to hich rabT nadare!" << endl;
    if ( buildingBlocksInFire.size() > 0 && (world->getTime() % actTime == 0/* || (world->getTime() < 16 && world->getTime() % 5 == 0*/ || myBuildingBlock == NULL))
    {
        isNearMatching = false;
        LOG(Main, 1) << "In jast ke darim sort mikonim va match!" << endl;
        setNeededFireBrigades();
        sortBuildingBlocks();
        matchMaker();
    }
    if(myBuildingBlock == NULL && buildingBlocksInFire.size() > 0)
    {
        matchToNearestFire();
    }
    if (buildingBlocksInFire.size() > 0 && world->getTime() % 8 == 0)
    {
        LOG(Main, 1) << "m1" << endl;
        if (myBuildingBlock != NULL)
        {
            LOG(Main, 1) << "m2" << endl;
            chooseMyBuilding();
            LOG(Main, 1) << "m2 -1 " << endl;
        }
    }

    if (myBuildingBlock != NULL)
    {
        if (myBuilding != NULL)
        {
            LOG(Main, 1) << "myBuildingBlock " << myBuildingBlock->buildings[0]->getId() << endl;
            LOG(Main, 1) << "myBuilding " << myBuilding->getId() << endl;
            LOG(Main, 1) << "m3" << endl;

            if (!positioning())
            {
                LOG(Main, 1) << "m4" << endl;
                extinguishPlan();
            }
            return;
        }
    }


    //SEARCH
    LOG(Main, 1) << "Searching..." << endl;
    if (targetForSearch != NULL && (self()->getMotionlessObject()->motionlessIndex == targetForSearch->motionlessIndex || !worldGraph->isReachable(self()->getRepresentiveNodeIndex(), targetForSearch->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED)))
        targetForSearch = NULL;
    if (targetForSearch == NULL)
        targetForSearch = search->actAmbulanceAndFireBrigadeSearch(canIGoRefuge);
    if (targetForSearch != NULL)
    {
        LOG(Main, 1) << "search target " << targetForSearch->getId() << endl;
        command->moveToMotionless(targetForSearch->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
        return;
    }
    for(int i = 0; i < world->buildings.size(); i++)
    {
        Building* b = world->buildings[i];
        if((b->isBurning() || (b->getTemperature() > 40 && b->getFieryness() != 8)) && world->getTime() - b->getLastCycleUpdated() < 15)
        {
            if(b->getPos().distFromPoint(self()->getPos()) < maxExtinguishDistance)
                command->extinguish(*b, maxWaterForExtinguish);
        }
    }

}

void FireBrigadeAgent::setBuildingBlocksInFire()
{
    LOG(Main, 1) << "setBuildingBlocksInFire begins" << endl;
    world->buildingBlocksInFire.clear();
    buildingBlocksInFire.clear();
    for (int i = 0; i < world->buildingBlocks.size(); i++)
    {
        for (int j = 0; j < world->buildingBlocks[i]->buildings.size(); j++)
            if (world->buildingBlocks[i]->buildings[j]->getTemperature() > 40 && world->buildingBlocks[i]->buildings[j]->getTemperature() != 8 && world->getTime() - world->buildingBlocks[i]->buildings[j]->getLastCycleUpdated() < 15)
            {
                world->buildingBlocksInFire.push_back(world->buildingBlocks[i]);
                LOG(Main, 1) << "bb in fire " << world->buildingBlocks[i]->buildings[0]->getId() << endl;
                break;
            }
    }
    burntBuildingBlocks.clear();
    for (int i = 0; i < world->buildingBlocks.size(); i++)
    {
        bool flag = true;
        for (int j = 0; j < world->buildingBlocks[i]->buildings.size(); j++)
        {
            if (world->buildingBlocks[i]->buildings[j]->getFieryness() != 8)
                flag = false;
        }
        if(flag)
            burntBuildingBlocks.insert(world->buildingBlocks[i]);
    }
    for (int i = 0; i < world->buildingBlocksInFire.size(); i++)
    {
        if (world->buildingBlocksInFire.size() < 6)
        {
            LOG(Main, 1) << "world->buildingBlocksInFire.size() < 10; bb in fire dded " << world->buildingBlocksInFire[i]->buildings[0]->getId() << endl;
            buildingBlocksInFire.push_back(world->buildingBlocksInFire[i]);
        }
        else if (isThereUnburnedBuildingBlockNear(world->buildingBlocksInFire[i]))
        {
            LOG(Main, 1) << "world->buildingBlocksInFire.size() > 10; bb in fire dded " << world->buildingBlocksInFire[i]->buildings[0]->getId() << endl;
            buildingBlocksInFire.push_back(world->buildingBlocksInFire[i]);
        }

    }
}

bool FireBrigadeAgent::isThereUnburnedBuildingBlockNear(BuildingBlock* bb)
{
    for (int i = 0; i < bb->buildings.size(); i++)
    {
        for (int j = 0; j < bb->buildings[i]->closerThan20B.size(); j++)
        {
            Building* b = bb->buildings[i]->closerThan20B[j];
            if (b->blockNum == bb->buildingBlockIndex)
                continue;
            bool flag = false;
            BuildingBlock* bb2 = world->buildingBlocks[b->blockNum];
            if(burntBuildingBlocks.find(bb2) != burntBuildingBlocks.end())
                continue;
            for (int k = 0; k < world->buildingBlocksInFire.size(); k++)
                if (world->buildingBlocksInFire[k]->buildingBlockIndex == bb2->buildingBlockIndex)
                    flag = true;
            if (!flag)
                return true;
        }
    }
    return false;
}

void FireBrigadeAgent::setNeededFireBrigades()
{
    for (int i = 0; i < buildingBlocksInFire.size(); i++)
    {
        int totalArea = 0;
        for (int j = 0; j < buildingBlocksInFire[i]->buildings.size(); j++)
            totalArea += buildingBlocksInFire[i]->buildings[j]->getAreaTotal();
        buildingBlocksInFire[i]->neededFirebBrigades = totalArea / 600;
        if (buildingBlocksInFire[i]->neededFirebBrigades == 0)
            buildingBlocksInFire[i]->neededFirebBrigades = 1;
        LOG(Main, 1) << "needed Fire brigades for " << buildingBlocksInFire[i]->buildings[0]->getId() << " is " << buildingBlocksInFire[i]->neededFirebBrigades << endl;
    }
}

void FireBrigadeAgent::sortBuildingBlocks()
{
    for (int i = 0; i < buildingBlocksInFire.size(); i++)
    {
        buildingBlocksInFire[i]->value = 0;

        if(world->buildingBlocksInFire.size() < 6)
        {
            int burningArea = 0, unburnedArea = 0;
            for (int j = 0; j < buildingBlocksInFire[i]->buildings.size(); j++)
            {
                Building* b = buildingBlocksInFire[i]->buildings[j];
                if ((b->isBurning() || (b->getTemperature() > 40 && b->getFieryness() != 8) && world->getTime() - b->getLastCycleUpdated() < 15))
                {
                    burningArea += b->getAreaTotal();
                }
                else if (b->getFieryness() != 8 && (!b->isBurning() || b->getTemperature() < 40))
                {
                    unburnedArea += b->getAreaTotal();
                }
            }
            if (burningArea != 0)
                buildingBlocksInFire[i]->value = unburnedArea / burningArea;
            else
                buildingBlocksInFire[i]->value = -1;
        } else
        {
            for(int j = 0; j < buildingBlocksInFire[i]->buildings.size(); j++)
            {
                Building* b = buildingBlocksInFire[i]->buildings[j];
                for(int k = 0; k < b->closerThan20B.size() ; k++)
                    if(!b->closerThan20B[k]->isBurning() && !b->getFieryness() != 8 && world->getTime() - b->closerThan20B[k]->getLastCycleUpdated() < 20)
                        buildingBlocksInFire[i]->value++;

            }
        }
    }
    sort(buildingBlocksInFire.begin(), buildingBlocksInFire.end(), bbComparator);
    LOG(Main, 1) << "best biuldingBlock: " << buildingBlocksInFire[0]->buildings[0]->getId() << endl;
}

void FireBrigadeAgent::matchMaker()
{
    myBuildingBlock = NULL;
    set<int> busyAgents;
    myTeammates.clear();
    for (int i = 0; i < buildingBlocksInFire.size(); i++)
    {
        int neededFireBrigades = buildingBlocksInFire[i]->neededFirebBrigades;
        for (int k = 0; k < neededFireBrigades; k++)
        {
            int minDist = MAX_INT;
            FireBrigade* minFb = NULL;
            for (int j = 0; j < world->fireBrigades.size(); j++)
            {
                FireBrigade* fb = world->fireBrigades[j];
                if (!fb->isAvailable)
                    continue;
                if (!fb->hasWater())
                    continue;
                if (busyAgents.find(fb->getId()) == busyAgents.end() && isReachableToBuildingBlock(fb, buildingBlocksInFire[i]))
                {
                    int minDist2 = MAX_INT;
                    FireBrigade* minFb2 = NULL;
                    for (int f = 0; f < buildingBlocksInFire[i]->buildings.size(); f++)
                    {
                        int dist = worldGraph->getDistance(buildingBlocksInFire[i]->buildings[f]->getRepresentiveNodeIndex(), fb->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
                        if (dist < minDist2)
                        {
                            minDist2 = dist;
                            minFb2 = fb;
                        }
                    }
                    if (minDist2 < minDist)
                    {
                        minDist = minDist2;
                        minFb = minFb2;
                    }
                }
            }
            if (minFb != NULL)
            {
                LOG(Main, 1) << "fb " << minFb->getId() << " mached to " << buildingBlocksInFire[i]->buildings[0]->getId() << endl;
                myTeammates.insert(minFb);
                busyAgents.insert(minFb->getId());
            }
        }
        if (myTeammates.find(self()) != myTeammates.end())
        {
            myBuildingBlock = buildingBlocksInFire[i];
            break;
        }
        else
            myTeammates.clear();
    }
}

void FireBrigadeAgent::matchToNearestFire()
{
    LOG(Main, 1) << "matching to nearest begins" << endl;
    int minDist = MAX_INT;
    BuildingBlock* minBB = NULL;
    for(int i = 0 ; i < buildingBlocksInFire.size(); i++)
    {
        for(int j = 0 ; j < buildingBlocksInFire[i]->buildings.size(); j++)
        {
            Building* b = buildingBlocksInFire[i]->buildings[j];
//            if(isReachableBuilding(self(), b))
            if(b->isBurning() || (b->getTemperature() > 40 && b->getFieryness() != 8)  && world->getTime() - b->getLastCycleUpdated() < 15 )
            {
                for(int k = 0; k < b->closerThan40M.size(); k++)
                {
                    if(worldGraph->isReachable(self()->getRepresentiveNodeIndex(), b->closerThan40M[k]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
                    {
                        int dist = worldGraph->getDistance(self()->getRepresentiveNodeIndex(), b->closerThan40M[k]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
                        if(dist < minDist)
                        {
                            minDist = dist;
                            minBB = buildingBlocksInFire[i];
                        }
                    }
                }
            }
        }
    }
    if(minBB != NULL)
    {
        LOG(Main , 1) << "i matched to nearest fire " << minBB->buildings[0]->getId() << endl;
        myBuildingBlock = minBB;
        isNearMatching = true;
    }
}

bool FireBrigadeAgent::isReachableToBuildingBlock(FireBrigade* fb, BuildingBlock* bb)
{
    for (int bCounter = 0; bCounter < bb->buildings.size(); bCounter++)
    {
        Building* thisBuilding = bb->buildings[bCounter];
        if ((thisBuilding->isBurning() || (thisBuilding->getTemperature() >= 40 && thisBuilding->getFieryness() != 8)) && world->getTime() - bb->buildings[bCounter]->getLastCycleUpdated() < 15 )
        {
            for (int cCounter = 0; cCounter < thisBuilding->mCloserThan40B.size(); cCounter++)
            {
                MotionlessObject* cMotionless = thisBuilding->mCloserThan40B[cCounter];
                if (worldGraph->isReachable(fb->getRepresentiveNodeIndex(), cMotionless->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void FireBrigadeAgent::eraseMyTargets()
{
    if (myBuildingBlock != NULL)
    {
        bool flag = true;
        for (int i = 0; i < buildingBlocksInFire.size(); i++)
            if (buildingBlocksInFire[i]->buildingBlockIndex == myBuildingBlock->buildingBlockIndex)
                flag = false;
        if (flag)
        {
            LOG(Main, 1) << "erase my target" << endl;
            myBuilding = NULL;
            myBuildingBlock = NULL;
            return;
        }
    }
}

void FireBrigadeAgent::chooseMyBuilding()
{
    //    srand(time(NULL));
    //    int myIndex = 0;
    //    for (set<FireBrigade*>::iterator it = myTeammates.begin(); it != myTeammates.end(); it++)
    //        if ((*it)->getId() < self()->getId())
    //            myIndex++;
    //    vector<Building*> buildingsInFire;
    //    vector<Building*> buildingsNotFire;
    //
    //    for (int i = 0; i < myBuildingBlock->buildings.size(); i++)
    //        if (world->getTime() - myBuildingBlock->buildings[i]->getLastCycleUpdated() < 15 && (myBuildingBlock->buildings[i]->getTemperature() > 40 && myBuildingBlock->buildings[i]->getFieryness() != 8) || myBuildingBlock->buildings[i]->isBurning())
    //            buildingsInFire.push_back(myBuildingBlock->buildings[i]);
    //        else if (myBuildingBlock->buildings[i]->getFieryness() != 8)
    //            buildingsNotFire.push_back(myBuildingBlock->buildings[i]);
    //
    //
    //    if (myIndex < buildingsInFire.size())
    //        myBuilding = buildingsInFire[myIndex];
    //    else if (myIndex - buildingsInFire.size() < buildingsNotFire.size())
    //        myBuilding = buildingsNotFire[myIndex - buildingsInFire.size()];
    //    else
    //        myBuilding = myBuildingBlock->buildings[rand() % myBuildingBlock->buildings.size()];
    //    LOG(Main, 1) << "myBuilding " << myBuilding->getId() << endl;
    myBuilding = NULL;
    if(isNearMatching)
    {
        int counter = 0, limit = 7;
        while(counter < limit && myBuilding == NULL)
        {
            int random = rand() % myBuildingBlock->buildings.size();
            Building* b = myBuildingBlock->buildings[random];
            if(isReachableBuilding(self(), b))
            {
                myBuilding = b;
                break;
            }
            counter++;
        }
    }
    if(myBuilding != NULL)
    {
        LOG(Main, 1) << "myBuilding " << myBuilding->getId() << endl;
        return;
    }

    LOG(Main, 1) << "choose buildingBlock2 " << endl;
    srand(time(NULL));
    vector<Building *> fireBuildings;
    fireBuildings.clear();

    for (int bCounter = 0; bCounter < myBuildingBlock->buildings.size(); bCounter++)
    {
        Building * thisBuilding = myBuildingBlock->buildings[bCounter];
        if (thisBuilding->getTemperature() >= 40 && thisBuilding->getFieryness() != 8)
        {
            int tempValue = 0;
            tempValue = 1000000 - thisBuilding->getAreaTotal() + thisBuilding->closerThan40B.size() * 400;
            thisBuilding->setValue(tempValue);
        }
        else if (thisBuilding->getFieryness() != 8)
        {
            int tempValue = thisBuilding->closerThan40B.size() * 800;
            thisBuilding->setValue(tempValue);
        }
        else
        {
            thisBuilding->setValue(-1);
        }
        if(thisBuilding->getFieryness() != 8 && isReachableBuilding(self(), thisBuilding))
            fireBuildings.push_back(thisBuilding);
    }

    LOG(Main, 1) << "tammoom shod hala!" << endl;

    std::sort(fireBuildings.begin(), fireBuildings.end(), bCompare);
    int myIndex = 0;
    for (set<FireBrigade*>::iterator it = myTeammates.begin(); it != myTeammates.end(); it++)
        if ((*it)->getId() < self()->getId())
            myIndex++;

    if(fireBuildings.size() != 0)
        myBuilding = fireBuildings[myIndex % fireBuildings.size()];
    if(myBuilding != NULL)
    {
        LOG(Main, 1) << "myBuilding " << myBuilding->getId() << endl;
        return;
    }
}

bool FireBrigadeAgent::positioning()
{
    //    set<int> occupiedBuidings;
    //    for (int i = 0; i < world->fireBrigades.size(); i++)
    //        if (world->fireBrigades[i]->fireBrigadeIndex != self()->fireBrigadeIndex)
    //            occupiedBuidings.insert(world->fireBrigades[i]->getMotionlessObject()->getId());
    for (int i = 0; i < myBuilding->getInSightPoints().size(); i++)
        if (self()->getMotionlessObject()->motionlessIndex == world->roads[myBuilding->getInSightPoints()[i].roadIndex]->motionlessIndex)
            return false;
    for (int i = 0; i < myBuilding->getInSightPoints().size(); i++)
    {
        if (!worldGraph->isReachable(self()->getRepresentiveNodeIndex(), world->roads[myBuilding->getInSightPoints()[i].roadIndex]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
            continue;
        LOG(Main, 1) << "myPosition: " << world->roads[myBuilding->getInSightPoints()[i].roadIndex]->getId() << endl;
        if (self()->getMotionlessObject()->motionlessIndex == world->roads[myBuilding->getInSightPoints()[i].roadIndex]->motionlessIndex)
        {
            LOG(Main, 1) << "return false 1" << endl;
            return false;
        }
        LOG(Main, 1) << "return true 1" << endl;
        command->moveToMotionless(world->roads[myBuilding->getInSightPoints()[i].roadIndex]->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
        return true;

    }
    for (int i = 0; i < myBuilding->closerThan40M.size(); i++)
        if(self()->getMotionlessObject()->motionlessIndex == myBuilding->closerThan40M[i]->motionlessIndex)
        {
            LOG(Main, 1) << "return false 1.5" << endl;
            return false;
        }

    int minDist = MAX_INT;
    MotionlessObject* minMotionless = NULL;
    for (int i = 0; i < myBuilding->closerThan40M.size(); i++)
        if (worldGraph->isReachable(self()->getRepresentiveNodeIndex(), myBuilding->closerThan40M[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED)/* && occupiedBuidings.find(myBuildingBlock->buildings[i]->getId()) == occupiedBuidings.end()*/)
        {
            int dist = worldGraph->getDistance(myBuilding->getRepresentiveNodeIndex(), myBuilding->closerThan40M[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
            if (dist < minDist)
            {
                minDist = dist;
                minMotionless = myBuilding->closerThan40M[i];
            }
        }
    if (minMotionless != NULL)
    {
        if (self()->getMotionlessObject()->motionlessIndex == minMotionless->motionlessIndex)
        {
            LOG(Main, 1) << "return false 2" << endl;
            return false;
        }
        else
        {
            LOG(Main, 1) << "return true 2" << endl;
            command->moveToMotionless(minMotionless->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
            return true;
        }
    }
    LOG(Main, 1) << "return false 3" << endl;
    return false;
}

void FireBrigadeAgent::extinguishPlan()
{
    if (myBuilding->isBurning() || (myBuilding->getTemperature() > 40 && myBuilding->getFieryness() != 8))
    {
        if (myBuilding->getTemperature() > 40 && myBuilding->getTemperature() < 230 && myBuilding->getFieryness() != 8 && world->getTime() - myBuilding->getLastCycleUpdated() < 7)
        {
            command->extinguish(*myBuilding, maxWaterForExtinguish);
            return;
        }
    }
    int numOfInFires = 0;
    for(int i = 0 ; i < self()->getMotionlessObject()->mCloserThan40B.size(); i++)
    {
        if(self()->getMotionlessObject()->mCloserThan40B[i]->isBuilding())
        {
            Building* b = (Building*)self()->getMotionlessObject()->mCloserThan40B[i];
            if(b->isBurning() || (b->getTemperature() > 40 && b->getFieryness() != 8)  && world->getTime() - b->getLastCycleUpdated() < 15)
                numOfInFires++;
        }
    }
//    for (int i = 0; i < self()->getMotionlessObject()->mCloserThan50B.size(); i++)
//    {
//        if (self()->getMotionlessObject()->mCloserThan50B[i]->isBuilding())
//        {
//            Building* b = (Building*) self()->getMotionlessObject()->mCloserThan50B[i];
//            if (b->getPos().distFromPoint(self()->getPos()) < maxExtinguishDistance && isInAImportantBuildingBlocks(b))
//            {
//                if (b->getTemperature() > 40 && b->getTemperature() < 230 && b->getFieryness() != 8 && world->getTime() - b->getLastCycleUpdated() < 7)
//                {
//                    command->extinguish(*b, maxWaterForExtinguish);
//                    return;
//                }
//            }
//        }
//    }

    vector<Building *> goodToExtinguish;
    goodToExtinguish.clear();

//    LOG(Main, 1) << "In jast !" << endl;

    for (int bCounter = 0; bCounter < self()->getMotionlessObject()->mCloserThan40B.size(); bCounter++)
    {
        if (self()->getMotionlessObject()->mCloserThan40B[bCounter]->isBuilding())
        {
            Building * thisBuilding = ((Building *) self()->getMotionlessObject()->mCloserThan40B[bCounter]);
            if (self()->getPos().distFromPoint(thisBuilding->getPos()) < maxExtinguishDistance &&
                    thisBuilding->getTemperature() >= 40 && (thisBuilding->getTemperature() < 230 || numOfInFires < 4) && thisBuilding->getFieryness() != 8 &&
                    isInAImportantBuildingBlocks(thisBuilding) )
            {

                int tempValue = 0;
                for(int i = 0; i < thisBuilding->closerThan30B.size(); i++)
                {
                    Building* b = thisBuilding->closerThan30B[i];
                    if(b->isUnburned() || b->isPutOut() || (b->getTemperature() < 40 && b->getTemperature() != 8))
                        tempValue++;
                }
                thisBuilding->setValue(tempValue);
                goodToExtinguish.push_back(thisBuilding);
            }
            else
            {
                thisBuilding->setValue(-1);
            }
        }
    }

    if (goodToExtinguish.size() >= 1)
    {
        command->extinguish(*goodToExtinguish[0], maxFireTank);
        return;
    }

    command->rest();
}

bool FireBrigadeAgent::isReachableBuilding(FireBrigade* fb, Building* b)
{
    for(int i = 0 ; i < b->closerThan40M.size(); i++)
        if(worldGraph->isReachable(fb->getRepresentiveNodeIndex(), b->closerThan40M[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED))
            return true;
    return false;
}

bool FireBrigadeAgent::isInAImportantBuildingBlocks(Building* b)
{
    BuildingBlock* bb = world->buildingBlocks[b->blockNum];
    int counter = 0, counter2 = 0;
    for(int i = 0 ; i < bb->buildings.size(); i++)
    {
        if(bb->buildings[i]->isBurning())
            counter++;
        else if(bb->buildings[i]->getFieryness() != 8)
            counter2++;
    }
    if(counter / bb->buildings.size() > .8)
        return false;
    else if(counter2 == 0)
        return false;
    for(int i = 0 ; i < buildingBlocksInFire.size(); i++)
    {
        if(buildingBlocksInFire[i]->buildingBlockIndex == bb->buildingBlockIndex)
            return true;
    }
    return false;
}

void FireBrigadeAgent::canGoRefuge()
{
    canIGoRefuge = false;
    for (int i = 0; i < world->refuges.size(); i++)
        if (worldGraph->isReachable(self()->getRepresentiveNodeIndex(), world->refuges[i]->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED))
        {
            canIGoRefuge = true;
            break;
        }
}

bool FireBrigadeAgent::needEscape()
{
    if (self()->getDamage() / self()->getHp() > .02 || self()->getDamage() > 90 || (self()->getHp() < 3500 && self()->getDamage() > 0))
        return true;
    return false;
}

bool FireBrigadeAgent::escape()
{
    if (needEscape())
    {
        if (canIGoRefuge)
        {
            moveToRefuges();
            return true;
        }
    }
    return false;
}

bool FireBrigadeAgent::dontShasWhenIAmNotReachable()
{
    if (self()->getRepresentiveNodeIndex() == -1)
    {
        LOG(Main, 1) << "in dontshas1" << endl;
        //        Polygon* polygon = &self()->getMotionlessObject()->getShape();
        //        Point middle = polygon->getMidPoint();
        //        int random = rand() % polygon->size();
        //        Point p2 = polygon->getVertex(random);
        //        Point randomPoint = Segment(middle, p2).getMiddlePoint();
        int randomNeighbour = rand() % self()->getMotionlessObject()->getNeighbours().size();
        vector<int> path;
        path.push_back(self()->getMotionlessObject()->getId());
        path.push_back(world->motionlessObjects[self()->getMotionlessObject()->getNeighbours()[randomNeighbour]]->getId());
        LOG(Main, 1) << "dontShasWhenIAmIsNotReachable " << world->motionlessObjects[self()->getMotionlessObject()->getNeighbours()[randomNeighbour]]->getId() << endl;
        command->move(path, -1, -1);
        return true;
    }
    else
        return false;
}

bool FireBrigadeAgent::dontShasWhenShasingInAMotionless()
{
    if (lastMotionless != NULL && self()->myLastCommand == CT_MOVE && lastMotionless == self()->getMotionlessObject())
    {
        if (!dontShas[0])
            dontShas[0] = true;
        else if (!dontShas[1])
            dontShas[1] = true;
        else if (!dontShas[2])
            dontShas[2] = true;
        else if (!dontShas[3])
            dontShas[3] = true;
    }
    else
    {
        dontShas[0] = dontShas[1] = dontShas[2] = dontShas[3] = false;
    }
    if (dontShas[0] && dontShas[1] && dontShas[2] && dontShas[3])
    {
        LOG(Main, 1) << "in dontshas2" << endl;
        dontShas[0] = dontShas[1] = dontShas[2] = dontShas[3] = false;
        command->moveToPoint(self()->getMotionlessObject()->motionlessIndex, self()->getMotionlessObject()->getPos(), GM_DEFAULT_NOT_BLOCKED);
        return true;
    }
    lastMotionless = self()->getMotionlessObject();
    return false;
}

bool FireBrigadeAgent::needToGoToRefuge()
{
    if (self()->getWaterQuantity() == 0)
        return true;
    return false;
}

bool FireBrigadeAgent::stayInRefuge()
{
    if (self()->getWaterQuantity() < maxFireTank)
        return true;
    return false;
}

void FireBrigadeAgent::moveToRefuges()
{
    LOG(Main, 1) << "move to refuges" << endl;
    int minDist = MAX_INT;
    Refuge* ref;
    for (int i = 0; i < world->refuges.size(); i++)
    {
        int dist = worldGraph->getDistance(self()->getRepresentiveNodeIndex(), world->refuges[i]->getRepresentiveNodeIndex(), GM_DEFAULT_NOT_BLOCKED);
        if (dist < minDist)
        {
            minDist = dist;
            ref = world->refuges[i];
        }
    }
    if (minDist != MAX_INT)
        command->moveToMotionless(ref->motionlessIndex, GM_DEFAULT_NOT_BLOCKED);
}

/********** Creating BuildingBlock **********/
void FireBrigadeAgent::createBuildingBlock()
{
    int color = 0;
    LOG(Main, 1) << "We start it" << endl;
    for (int i = 0; i < world->buildings.size(); i++)
    {
        if (world->buildings[i]->blockNum == -1)
        {
            BuildingBlock * a = new BuildingBlock();
            a->buildingBlockIndex = color;
            dfs(a, color, world->buildings[i]);
            LOG(Main, 2) << "Hey DFS finished" << " and :       " << color << endl;
            world->buildingBlocks.push_back(a);
            color++;
        }
    }
}

bool FireBrigadeAgent::isHere(vector<Road*> a, int b)
{
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i]->getId() == b)
            return true;
    }
    return false;
}


void FireBrigadeAgent::dfs(BuildingBlock* buildingBlock, int color, Building* building)
{
    buildingBlock->buildings.push_back(building);
    building->blockNum = color;
    for (int i = 0; i < building->closerThan50B.size(); i++)
    {
        if (building->closerThan50B[i]->blockNum == -1 &&
                isNeighbor(building, building->closerThan50B[i]))
        {
            dfs(buildingBlock, color, building->closerThan50B[i]);
        }
    }
}

bool FireBrigadeAgent::isNeighbor(Building* a, Building* b)
{
    int thisPoint;
    int nextPoint;

    vector<Road *> rCheck;

    for (int i = 0; i < a->closerThan100R.size(); i++)
    {
        if (a->closerThan100R[i]->getPos().distFromPoint(b->getPos()) < 70000)
        {
            rCheck.push_back(a->closerThan100R[i]);
        }
    }

    //    LOG(Main, 1) << "rCheck.size() is " << rCheck.size() << endl;

    Segment aAndB(a->getPos(), b->getPos());
    for (int rCounter = 0; rCounter < rCheck.size(); rCounter++)
    {
        for (int rPCounter = 0; rPCounter < rCheck[rCounter]->getShape().getVertices().size(); rPCounter++)
        {
            thisPoint = rPCounter;
            nextPoint = rPCounter + 1;

            if (thisPoint == (rCheck[rCounter]->getShape().getVertices().size() - 1))
            {
                nextPoint = 0;
            }

            Segment roadLine(rCheck[rCounter]->getShape().getVertices()[thisPoint],
                             rCheck[rCounter]->getShape().getVertices()[nextPoint]);

            bool a = isIntersect(aAndB, roadLine);

            if (a)
            {
                return false;
            }
        }
    }
    return true;

}

void FireBrigadeAgent::updateBuildingBlocks()
{
    for (int i = 0; i < world->buildingBlocks.size(); i++)
    {
        world->buildingBlocks[i]->update();
        LOG(Main, 1) << "center of " << world->buildingBlocks[i]->buildings[0]->getId() << " is " << world->buildingBlocks[i]->getCenter()->getId() << endl;
    }
}

FireBrigade* FireBrigadeAgent::self()
{
    return (FireBrigade*) world->self;
}