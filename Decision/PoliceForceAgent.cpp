/* 
 * File:   PoliceForceAgent.cpp
 * Author: Alimohammad
 * 
 * Created on September 24, 2010, 3:59 PM
 */

#include "PoliceForceAgent.h"
#include "../Utilities/Debugger.h"
#include "../Utilities/Geometry/GeometryFunctions.h"
#include <cstdlib>

#define LOGLEVEL 1
using namespace Types;
using namespace std;
using namespace Geometry;
const int maxDistanceForBeingInOneTribe = 200000;
//const int maxDistanceForBeingInOneTribe = 15000;
const int maxDistanceBetweenTwoMotionlessInOneTribe = 300000;
//const int maxDistanceBetweenTwoMotionlessInOneTribe = 30000;

PoliceForceAgent::PoliceForceAgent()
{
}

PoliceForceAgent::~PoliceForceAgent()
{
}

bool firstTribesComparator(std::set<MotionlessObject*> v1, std::set<MotionlessObject*> v2)
{
    return v1.size() > v2.size();
}

bool XYComparator(MotionlessObject* mo1, MotionlessObject* mo2)
{
    return mo1->getX() + mo1->getY() < mo2->getX() + mo2->getY();
}

void PoliceForceAgent::precomputation()
{
    Agent::precomputation();
    maxClearDistance = atoi(world->config[Encodings::getConfigType(CT_CLEAR_DISTANCE)].c_str());
    repairRate = atoi(world->config[Encodings::getConfigType(CT_REPAIR_RATE)].c_str());
    targetForSearch = NULL;
    targetForUpdate = NULL;
    lastMotionless = NULL;
    dontShas[0] = dontShas[1] = dontShas[2] = dontShas[3] = false;

    //    Building *a = (Building *) (world->objects[944]);
    //    Building *b = (Building *) (world->objects[943]);
    //    //    Building *c = (Building *) (world->objects[935]);
    //
    //    LOG(Main, 1) << "AB KIR:    " << isNeighbor(a, b) << endl;

    LOG(Main, 2) << "before create Building Blocks" << endl;
    createBuildingBlock();
    LOG(Main, 2) << "after create Building Blocks" << endl;
    for (int i = 0; i < world->buildingBlocks.size(); i++)
        world->buildingBlocks[i]->update();
    int color = 0;
    for (int i = 0; i < world->buildingBlocks.size(); i++)
    {
        for (int j = 1; j < world->buildingBlocks[i]->buildings.size(); j++)
        {
            LOG(Main, 1) << "line" << world->buildingBlocks[i]->buildings[j - 1]->getPos() << world->buildingBlocks[i]->buildings[j]->getPos() << " " << color << "; ";
        }
        if (color == 4)
            color = 0;
        else
            color++;
    }
    LOG(Main, 1) << endl;


    std::vector<MotionlessObject *> firstTargets;
    for (int i = 0; i < world->humans.size(); i++)
        if (!world->humans[i]->isPoliceForce())
            firstTargets.push_back(world->humans[i]->getMotionlessObject());
    for (int i = 0; i < world->refuges.size(); i++)
        firstTargets.push_back(world->refuges[i]);
    sort(firstTargets.begin(), firstTargets.end(), XYComparator);

    set<MotionlessObject*> marked;
    for (int i = 0; i < firstTargets.size(); i++)
    {
        if (marked.find(firstTargets[i]) == marked.end())
        {
            LOG(Main, 2) << "First Target " << firstTargets[i]->getId() << " was not marked" << endl;
            set<MotionlessObject*> currentTargets;
            setFirstTribes(firstTargets[i], currentTargets, marked, firstTargets);
            firstTribes.push_back(currentTargets);
        }
        LOG(Main, 2) << "marked: " << endl;
        //        for(set<MotionlessObject*>::iterator it = marked.begin(); it != marked.end(); it++)
        //            LOG(Main, 2) << " " << (*it)->getId();
        LOG(Main, 2) << endl;
    }

    sort(firstTribes.begin(), firstTribes.end(), firstTribesComparator);

    for (int i = 0; i < firstTribes.size(); i++)
    {
        LOG(Main, 1) << "new tribe:" << endl;
        for (set<MotionlessObject*>::iterator it = firstTribes[i].begin(); it != firstTribes[i].end(); it++)
            LOG(Main, 1) << " " << (*it)->getId();
        LOG(Main, 1) << endl;
    }
    //    MotionlessObject * lastDuties[world->policeForces.size()];
    //    for (int i = 0; i < world->policeForces.size(); i++)
    //        lastDuties[i] = world->policeForces[i]->getMotionlessObject();
    //    matchPolicesToFirstTargets(lastDuties);
    //    //    for(int i = 0; i < world->policeForces.size(); i++)
    //    //        LOG(Main, 1) << "policeForce with id " << world->policeForces[i]->getId() << " is going to " << lastDuties[i]->getId() << endl;
    //    for (int i = 0; i < myFirstTribesIndex.size(); i++)
    //        LOG(Main, 1) << "my first target index: " << myFirstTribesIndex[i] << endl;
    //    search->init(lastDuties);
}

void PoliceForceAgent::precompute()
{
    MotionlessObject * lastDuties[world->policeForces.size()];
    for (int i = 0; i < world->policeForces.size(); i++)
        lastDuties[i] = world->policeForces[i]->getMotionlessObject();
    matchPolicesToFirstTargets(lastDuties);
        for(int i = 0; i < world->policeForces.size(); i++)
            LOG(Main, 1) << "policeForce with id " << world->policeForces[i]->getId() << " is going to " << lastDuties[i]->getId() << endl;
    for (int i = 0; i < myFirstTribesIndex.size(); i++)
        LOG(Main, 1) << endl <<" my first target index: " << myFirstTribesIndex[i] << endl;
    search->init(lastDuties);
}

void PoliceForceAgent::setFirstTribes(MotionlessObject* target, std::set<MotionlessObject*> &currentTargets, std::set<MotionlessObject*> &addedTargets, std::vector<MotionlessObject*> &allFirstTargets)
{
    currentTargets.insert(target);
    addedTargets.insert(target);
    for (int i = 0; i < allFirstTargets.size(); i++)
    {
        if (addedTargets.find(allFirstTargets[i]) == addedTargets.end())
        {
            int dist = MAX_INT;
            if (!target->isRefuge())
                dist = worldGraph->getDistance(target->getWithoutBlockadeRepresentiveNodeIndex(), allFirstTargets[i]->getRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
            else if (!allFirstTargets[i]->isRefuge())
                dist = worldGraph->getDistance(allFirstTargets[i]->getWithoutBlockadeRepresentiveNodeIndex(), target->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
            else
                continue;

            LOG(Main, 2) << "dist between " << target->getId() << " & " << allFirstTargets[i]->getId() << " = " << dist << endl;

            if (dist < maxDistanceForBeingInOneTribe)
            {
                bool flag = true;
                for (set<MotionlessObject*>::iterator it = currentTargets.begin(); it != currentTargets.end(); it++)
                {
                    int dist2 = worldGraph->getDistance(target->getWithoutBlockadeRepresentiveNodeIndex(), (*it)->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
                    LOG(Main, 2) << "dist2 = " << dist2 << endl;
                    if (dist2 > maxDistanceBetweenTwoMotionlessInOneTribe)
                    {
                        flag = false;
                        break;
                    }
                }
                if (flag)
                {
                    LOG(Main, 2) << "flag is true" << endl;
                    setFirstTribes(allFirstTargets[i], currentTargets, addedTargets, allFirstTargets);
                }
            }
        }
    }

}

void PoliceForceAgent::matchPolicesToFirstTargets(MotionlessObject* lastDuties[])
{
    //maxCapacity means the each can save max how many tribes
    int maxCapacity;
    if (firstTribes.size() % world->policeForces.size() == 0)
        maxCapacity = firstTribes.size() / world->policeForces.size();
    else
        maxCapacity = firstTribes.size() / world->policeForces.size() + 1;
    LOG(Main, 1) << "Max Capacity = " << maxCapacity << endl;
    int numOfDuties[world->policeForces.size()];
    std::fill(numOfDuties, numOfDuties + world->policeForces.size(), 0);
    myFirstTribesIndex.clear();
    //    set<PoliceForce*> hasJob;
    for (int i = 0; i < firstTribes.size(); i++)
    {
        int minDist = MAX_INT;
        PoliceForce* minPF;
        for (set<MotionlessObject*>::iterator it = firstTribes[i].begin(); it != firstTribes[i].end(); it++)
        {
            for (int k = 0; k < world->policeForces.size(); k++)
            {
                if (world->policeForces[k]->getBuriedness() > 0)
                    continue;
                int dist = worldGraph->getDistance(world->policeForces[k]->getWithoutBlockadeRepresentiveNodeIndex(), (*it)->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
                //                if(dist < minDist && hasJob.find(world->policeForces[k]) == hasJob.end())
                if (dist < minDist && numOfDuties[world->policeForces[k]->policeForceIndex] < maxCapacity)
                {
                    minDist = dist;
                    minPF = world->policeForces[k];
                }
            }
        }
        if (minPF->policeForceIndex == self()->policeForceIndex)
        {
            myFirstTribesIndex.push_back(i);

        }

        lastDuties[minPF->policeForceIndex] = *(firstTribes[i].begin());
        LOG(Main, 1) << "line" << minPF->getPos() << (*(firstTribes[i].begin()))->getPos() << " 1; ";

        //        else
        //        {
        numOfDuties[minPF->policeForceIndex]++;
        ////            hasJob.insert(minPF);
        //        }
    }
}

void PoliceForceAgent::setSecondaryTribes()
{
    LOG(Main, 1) << "setSecondaryTribes begins" << endl;
    secondaryTribes.clear();
    //    if (selfRepresentativeNode == -1)
    //        selfRepresentativeNode = self()->getWithoutBlockadeRepresentiveNodeIndex();
    for (int i = 0; i < world->buildingBlocksInFire.size(); i++)
    {
        bool flag = true;
        for (int j = 0; j < world->buildingBlocksInFire[i]->buildings.size(); j++)
        {
            for (int k = 0; k < world->refuges.size(); k++)
            {
                //                LOG(Main, 1) << "buildingblock in fire; building " << world->buildingBlocksInFire[i]->buildings[j]->getId() << " " << worldGraph->isReachable(world->refuges[k]->getRepresentiveNodeIndex(), world->buildingBlocksInFire[i]->buildings[j]->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED) << endl;
                if (worldGraph->isReachable(world->refuges[k]->getRepresentiveNodeIndex(), world->buildingBlocksInFire[i]->buildings[j]->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED))
                {
                    flag = false;
                    break;
                }
            }
            if (!flag)
                break;
        }
        if (flag)
            secondaryTribes.push_back(world->buildingBlocksInFire[i]);
    }
}

bool PoliceForceAgent::clear(MotionlessObject* target)
{
    if (self()->getMotionlessObject()->isRoad() && self()->getMotionlessObject()->getInnerHumansIndexes().size() > 1)
    {
        LOG(Main, 1) << "inner human cleaning! size: " << self()->getMotionlessObject()->getInnerHumansIndexes().size() << endl;
        for (int i = 0; i < self()->getMotionlessObject()->getInnerHumansIndexes().size(); i++)
        {
            Human *agent = world->humans[self()->getMotionlessObject()->getInnerHumansIndexes()[i]];
            if (agent->isFireBrigade() || agent->isAmbulanceTeam())
            {
                Segment seg(self()->getPos(), agent->getPos());
                Road *r = (Road*) self()->getMotionlessObject();
                Blockade *b = NULL;
                double minDist = 1e15;
                for (int i = 0; i < (int) r->getBlockades().size(); i++)
                {
                    double minD = 1e20;
                    for (int j = 0; j < r->getBlockades()[i]->getShape().size(); j++)
                    {
                        if (isIntersect(seg, r->getBlockades()[i]->getShape().getSegment(j)))
                        {
                            Point interPoint = getIntersectPoint(seg.asLine(), r->getBlockades()[i]->getShape().getSegment(j).asLine());
                            double dist = distanceBetweenPoints(seg.getFirstPoint(), interPoint);
                            if (isLess(dist, minD))
                            {
                                minD = dist;
                            }
                        }
                    }
                    if (isLess(minD, minDist))
                    {
                        minDist = minD;
                        b = r->getBlockades()[i];
                    }
                }
                if (b != NULL)
                {
                    double minD = 1e20;
                    for (int i = 0; i < b->getShape().size(); i++)
                    {
                        double d1 = distanceBetweenPoints(self()->getPos(), b->getShape().getVertex(i));
                        if (isLess(d1, minD))
                        {
                            minD = d1;
                        }
                        //		Point per = getPerpendicularPoint(r->getShape().getSegment(i).asLine(), self()->getPos());
                        //		if(isOnSegment(r->getShape().getSegment(i), per))
                        //		{
                        //			double d2 = distanceBetweenPoints(self()->getPos(), per);
                        //			if(isLess(d2, minD))
                        //			{
                        //				minD = d2;
                        //			}
                        //		}
                    }
                    if (isLess(minD, (double) maxClearDistance))
                    {
                        LOG(Main, 1) << "clear human " << agent->getId() << endl;
                        command->clear(*b);
                        return true;
                    }
                    return false;
                }
            }
        }
    }
    if (self()->getMotionlessObject()->isRoad())
    {
        LOG(Main, 1) << "in clear nodes" << endl;
        for (int i = 0; i < (int) self()->getMotionlessObject()->getRelativeNodesIndexes().size(); i++)
        {
            Segment seg(self()->getPos(), worldGraph->getNodes()[self()->getMotionlessObject()->getRelativeNodesIndexes()[i]]->getRepresentivePoint());
            Road *r = (Road*) self()->getMotionlessObject();
            Blockade *b = NULL;
            double minDist = 1e15;
            for (int i = 0; i < (int) r->getBlockades().size(); i++)
            {
                double minD = 1e20;
                for (int j = 0; j < r->getBlockades()[i]->getShape().size(); j++)
                {
                    if (isIntersect(seg, r->getBlockades()[i]->getShape().getSegment(j)))
                    {
                        Point interPoint = getIntersectPoint(seg.asLine(), r->getBlockades()[i]->getShape().getSegment(j).asLine());
                        double dist = distanceBetweenPoints(seg.getFirstPoint(), interPoint);
                        if (isLess(dist, minD))
                        {
                            minD = dist;
                        }
                    }
                }
                if (isLess(minD, minDist))
                {
                    minDist = minD;
                    b = r->getBlockades()[i];
                }
            }
            if (b != NULL)
            {
                double minD = 1e20;
                for (int i = 0; i < b->getShape().size(); i++)
                {
                    double d1 = distanceBetweenPoints(self()->getPos(), b->getShape().getVertex(i));
                    if (isLess(d1, minD))
                    {
                        minD = d1;
                    }
                    //		Point per = getPerpendicularPoint(r->getShape().getSegment(i).asLine(), self()->getPos());
                    //		if(isOnSegment(r->getShape().getSegment(i), per))
                    //		{
                    //			double d2 = distanceBetweenPoints(self()->getPos(), per);
                    //			if(isLess(d2, minD))
                    //			{
                    //				minD = d2;
                    //			}
                    //		}
                }
                if (isLess(minD, (double) maxClearDistance))
                {
                    LOG(Main, 1) << "clear node " << world->motionlessObjects[worldGraph->getNodes()[self()->getMotionlessObject()->getRelativeNodesIndexes()[i]]->getFirstMotionlessIndex()]->getId() << ' ' << world->motionlessObjects[worldGraph->getNodes()[self()->getMotionlessObject()->getRelativeNodesIndexes()[i]]->getSecondMotionlessIndex()]->getId() << endl;
                    command->clear(*b);
                    return true;
                }
            }
        }
    }
    vector<int> path = worldGraph->getPath(self()->getWithoutBlockadeRepresentiveNodeIndex(), target->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
    if (path.size() == 0)
    {
        return false;
    }
    if (path.size() >= 1)
    {
        Node* node = worldGraph->getNodes()[path[1]];
        if (node->getFirstMotionlessIndex() == self()->getMotionlessObject()->motionlessIndex || node->getSecondMotionlessIndex() == self()->getMotionlessObject()->motionlessIndex)
        {
            path.erase(path.begin());
        }
    }
    Edge *impassableEdge = NULL;
    bool isFirst = false;
    //	for(int i = 0; i < (int)path.size(); i++)
    //	{
    //		LOG(Main, 1) << path[i] << ' ';
    //	}
    //	LOG(Main, 1) << endl;
    for (int i = 0; i < (int) path.size() - 1; i++)
    {
        Node* node1 = worldGraph->getNodes()[path[i]];
        int edgeIndex = -1;
        for (int j = 0; j < (int) node1->getNeighbours().size(); j++)
        {
            if (node1->getNeighbours()[j] == path[i + 1])
            {
                edgeIndex = node1->getEdgeIndexes()[j];
                break;
            }
        }
        if (edgeIndex != -1)
        {
            if (worldGraph->getEdges()[edgeIndex]->getPassingMode() == PM_NOT_PASSABLE)
            {
                impassableEdge = worldGraph->getEdges()[edgeIndex];
                if (impassableEdge->getMotionlessIndex() == path[i])
                {
                    isFirst = true;
                }
                break;
            }
        }
    }
    if (impassableEdge == NULL || world->motionlessObjects[impassableEdge->getMotionlessIndex()]->isBuilding())
    {
        return false;
    }
    //	LOG(Main, 1) << impassableEdge->getFirstNodeIndex() << ' ' << impassableEdge->getSecondNodeIndex() << endl;
    Road *r = (Road*) world->motionlessObjects[impassableEdge->getMotionlessIndex()];
    Segment edge;
    if (isFirst)
    {
        edge.setFirstPoint(worldGraph->getNodes()[impassableEdge->getFirstNodeIndex()]->getRepresentivePoint());
        edge.setSecondPoint(worldGraph->getNodes()[impassableEdge->getSecondNodeIndex()]->getRepresentivePoint());
    }
    else
    {
        edge.setFirstPoint(worldGraph->getNodes()[impassableEdge->getSecondNodeIndex()]->getRepresentivePoint());
        edge.setSecondPoint(worldGraph->getNodes()[impassableEdge->getFirstNodeIndex()]->getRepresentivePoint());
    }
    Blockade *b = NULL;
    double minDist = 1e15;
    for (int i = 0; i < (int) r->getBlockades().size(); i++)
    {
        double minD = 1e20;
        for (int j = 0; j < r->getBlockades()[i]->getShape().size(); j++)
        {
            if (isIntersect(edge, r->getBlockades()[i]->getShape().getSegment(j)))
            {
                Point interPoint = getIntersectPoint(edge.asLine(), r->getBlockades()[i]->getShape().getSegment(j).asLine());
                double dist = distanceBetweenPoints(edge.getFirstPoint(), interPoint);
                if (isLess(dist, minD))
                {
                    minD = dist;
                }
            }
        }
        if (isLess(minD, minDist))
        {
            minDist = minD;
            b = r->getBlockades()[i];
        }
    }
    if (b == NULL)
    {
        if (r->getBlockades().size() > 0)
        {
            b = r->getBlockades()[0];
        }
        else
        {
            return false;
        }
    }
    double minD = 1e20;
    for (int i = 0; i < b->getShape().size(); i++)
    {
        double d1 = distanceBetweenPoints(self()->getPos(), b->getShape().getVertex(i));
        if (isLess(d1, minD))
        {
            minD = d1;
        }
        //		Point per = getPerpendicularPoint(r->getShape().getSegment(i).asLine(), self()->getPos());
        //		if(isOnSegment(r->getShape().getSegment(i), per))
        //		{
        //			double d2 = distanceBetweenPoints(self()->getPos(), per);
        //			if(isLess(d2, minD))
        //			{
        //				minD = d2;
        //			}
        //		}
    }
    if (isLess(minD, (double) maxClearDistance))
    {
        LOG(Main, 1) << "clearing edge " << world->motionlessObjects[impassableEdge->getMotionlessIndex()]->getId() << endl;
        command->clear(*b);
        return true;
    }
    return false;
}

void PoliceForceAgent::setBuildingBlocksInFire()
{
    world->buildingBlocksInFire.clear();
    for (int i = 0; i < world->buildingBlocks.size(); i++)
    {
        for (int j = 0; j < world->buildingBlocks[i]->buildings.size(); j++)
        {
            if (world->buildingBlocks[i]->buildings[j]->isBurning())
            {
                world->buildingBlocksInFire.push_back(world->buildingBlocks[i]);
                break;
            }
        }
    }
}

BuildingBlock* PoliceForceAgent::chooseMySecondaryTribe()
{
    LOG(Main, 1) << "chooseMySecondaryTribe begins" << endl;
    set<int> busyPolices;
    for (int i = 0; i < secondaryTribes.size(); i++)
    {
        int minDist = MAX_INT;
        PoliceForce* pf = NULL;
        LOG(Main, 1) << "secondary tribe " << secondaryTribes[i]->getCenter()->getId() << endl;
        for (int j = 0; j < world->policeForces.size(); j++)
        {
            //            LOG(Main, 1) << "policeForce " << world->policeForces[j]->getId() << endl;
            int dist = worldGraph->getDistance(world->policeForces[j]->getWithoutBlockadeRepresentiveNodeIndex(), secondaryTribes[i]->getCenter()->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
            //            LOG(Main, 1) << "dist is " << dist << endl;
            if (dist < minDist && busyPolices.find(world->policeForces[j]->policeForceIndex) == busyPolices.end())
            {
                minDist = dist;
                pf = world->policeForces[j];
            }
        }
        if (pf != NULL)
        {
            if (pf->policeForceIndex == self()->policeForceIndex)
            {
                return secondaryTribes[i];
            }
            else
            {
                busyPolices.insert(pf->policeForceIndex);
            }
        }
    }
    return NULL;
}

void PoliceForceAgent::actBeforeRadar()
{
}

void PoliceForceAgent::act()
{
    LOG(Main, 1) << "I am policeforce" << endl;
    LOG(Main, 1) << "my Position: " << self()->getMotionlessObject()->getId() << endl;
    LOG(Main, 1) << "my rep: " << self()->getRepresentiveNodeIndex() << endl;
    if (world->getTime() <= ignoreCommandsUntil + 1)
    {
        lastPosition = self()->getPos();
        return;
    }
    if(world->getTime() == ignoreCommandsUntil + 2)
        precompute();
//    command->moveToMotionless(((MotionlessObject*)world->objects[975])->motionlessIndex, GM_WITHOUT_BLOCKADE);
    //    for (int i = 0; i < worldGraph->getEdges().size(); i++)
    //    {
    //        Edge *edge = worldGraph->getEdges()[i];
    //        switch (edge->getPassingMode())
    //        {
    //        case PM_PASSABLE:
    //            setColor(0);
    //            break;
    //        case PM_NOT_PASSABLE:
    //            setColor(4);
    //            break;
    //        case PM_UNKNOWN:
    //            setColor(1);
    //            break;
    //        }
    //        LOG(Main, 1) << Segment(worldGraph->getNodes()[edge->getFirstNodeIndex()]->getRepresentivePoint(), worldGraph->getNodes()[edge->getSecondNodeIndex()]->getRepresentivePoint()) << ' ';
    //    }
    //    LOG(Main, 1) << endl;

    //    for(int i=0; i < firstTribes.size(); i++)
    //    {
    //        LOG(Main, 1) << "new tribe:" << endl;
    //        for(set<MotionlessObject*>::iterator it = firstTribes[i].begin() ; it != firstTribes[i].end(); it++)
    //            LOG(Main, 1) << " " << (*it)->getId();
    //        LOG(Main, 1) << endl;
    //    }
    for (int i = 0; i < world->buildings.size(); i++)
        if (world->buildings[i]->iHaveBeenHere)
            LOG(Main, 1) << "circle" << world->buildings[i]->getPos() << " 2500, 2; ";
    LOG(Main, 1) << endl;
    if (dontShasWhenShasingInAPoint())
        return;
    //    if (dontShasWhenIAmNotReachable())
    //        return;
    if (dontShasWhenShasingInAMotionless())
        return;
    // can I go to any refgue
    canGoRefuge();

    if (!canIGoRefuge && self()->getMotionlessObject()->isBuilding() && ((Building*) self()->getMotionlessObject())->isBurning())
    {
        command->moveToMotionless(world->roads[rand() % world->roads.size()]->motionlessIndex, GM_WITHOUT_BLOCKADE);
    }
    LOG(Main, 1) << "after canGoRefuge " << canIGoRefuge << endl;
    //escape
    if (escape())
        return;
    LOG(Main, 1) << "after escape" << endl;

    setBuildingBlocksInFire();
    if (saveCivilians())
        return;
    savePolices();

    if (world->refuges.size() > 0)
    {

        setSecondaryTribes();
        LOG(Main, 1) << "Number of Secondary Tribes: " << secondaryTribes.size() << endl;
        BuildingBlock* mySecondaryTribe = chooseMySecondaryTribe();
        if (mySecondaryTribe != NULL)
        {
            LOG(Main, 1) << "my secondary tribe is " << mySecondaryTribe->buildings[0]->getId() << endl;
            if (clearMySecondTribe(mySecondaryTribe))
                return;
        }
    }
    //-1 means i dont have first target
    if (myFirstTribesIndex.size() != 0)
    {
        LOG(Main, 1) << "In the if1" << endl;
        updateMyFirstTribe();
        for (int i = 0; i < myFirstTribesIndex.size(); i++)
        {
            LOG(Main, 1) << "myfirstTribeIndex: " << i << endl;
            if (firstTribes[myFirstTribesIndex[i]].size() > 0)
            {
                LOG(Main, 1) << "In the if2" << endl;
                if (clearMyFirstTribe(i))
                    return;
            }
        }

    }
    LOG(Main, 1) << "Searching..." << endl;
    if (targetForSearch != NULL && (self()->getMotionlessObject()->motionlessIndex == targetForSearch->motionlessIndex || (!canIGoRefuge && targetForSearch->isBurning())))
        targetForSearch = NULL;
    if (targetForSearch == NULL)
        targetForSearch = search->actPoliceSearch(canIGoRefuge);
    if (targetForSearch != NULL)
    {
        if (clear((MotionlessObject*) targetForSearch))
            return;
        command->moveToMotionless(targetForSearch->motionlessIndex, GM_WITHOUT_BLOCKADE);
        return;
    }
    LOG(Main, 1) << "I searched all my buildings; Now I update" << endl;
    if (targetForUpdate != NULL && self()->getMotionlessObject()->motionlessIndex == targetForUpdate->motionlessIndex)
        targetForUpdate = NULL;
    if (targetForUpdate == NULL)
        targetForUpdate = search->updateCiviliansAndZone(canIGoRefuge);
    if (targetForUpdate != NULL)
    {
        if (clear((MotionlessObject*) targetForUpdate))
            return;
        command->moveToMotionless(targetForUpdate->motionlessIndex, GM_WITHOUT_BLOCKADE);
        return;
    }
    LOG(Main, 1) << "I don't have any duty; Why? Probably there is a problem!" << endl;
    if (!self()->getMotionlessObject()->isRefuge())
        moveToRefuges();
    else
        command->rest();
    LOG(Main, 1) << "END" << endl;
}

bool PoliceForceAgent::clearMyFirstTribe(int tribendex)
{
    int minDist = MAX_INT;
    MotionlessObject* minMO;
    for (set<MotionlessObject*>::iterator it = firstTribes[myFirstTribesIndex[tribendex]].begin(); it != firstTribes[myFirstTribesIndex[tribendex]].end(); it++)
    {
        int dist = worldGraph->getDistance(self()->getWithoutBlockadeRepresentiveNodeIndex(), (*it)->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
        if (dist < minDist)
        {
            minDist = dist;
            minMO = *it;
        }
    }
    if (minDist != MAX_INT)
    {
        if (clear(minMO))
            return true;
        command->moveToMotionless(minMO->motionlessIndex, GM_WITHOUT_BLOCKADE);
        return true;
    }
    return false;
}

bool PoliceForceAgent::clearMySecondTribe(BuildingBlock* tribe)
{
    LOG(Main, 1) << "clear my second tribe begins" << endl;
    Building* target = NULL;
    int selfRepresentativeNode = self()->getRepresentiveNodeIndex();
    //    if (selfRepresentativeNode == -1)
    //        selfRepresentativeNode = self()->getWithoutBlockadeRepresentiveNodeIndex();

    for (int i = 0; i < tribe->buildings.size(); i++)
    {
        if (tribe->buildings[i]->isBurning())
        {
            LOG(Main, 1) << "reachablity between me(" << selfRepresentativeNode << ") and building " << tribe->buildings[i]->getId() << " with representative node " << tribe->buildings[i]->getRepresentiveNodeIndex() << " is " << worldGraph->isReachable(selfRepresentativeNode, tribe->buildings[i]->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED) << endl;
            if (!worldGraph->isReachable(selfRepresentativeNode, tribe->buildings[i]->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED))
            {
                target = tribe->buildings[i];
                break;
            }
        }
    }
    if (target == NULL && world->refuges.size() > 0)
    {
        moveToRefuges();
        return true;
    }
    else if (target != NULL)
    {
        if (clear(target))
        {
            return true;
        }
        if (!canIGoRefuge && target->isBurning())
        {
            for (int i = 0; i < target->getNeighbours().size(); i++)
            {
                if (world->motionlessObjects[target->getNeighbours()[i]]->isRoad())
                {
                    command->moveToMotionless(target->motionlessIndex, GM_WITHOUT_BLOCKADE);
                    return true;
                }
            }
            return false;
        }
        command->moveToMotionless(target->motionlessIndex, GM_WITHOUT_BLOCKADE);
        return true;
    }
    return false;
}

void PoliceForceAgent::updateMyFirstTribe()
{
    LOG(Main, 1) << "Start Of updateMyFirstTribe" << endl;
    for (int i = 0; i < myFirstTribesIndex.size(); i++)
    {
        for (set<MotionlessObject*>::iterator it = firstTribes[myFirstTribesIndex[i]].begin(); it != firstTribes[myFirstTribesIndex[i]].end(); it++)
        {
            //            if (worldGraph->isReachable(self()->getRepresentiveNodeIndex(), (*it)->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED) || ((*it)->isBuilding() && ((Building*) (*it))->isBurning() && world->refuges.size() == 0))
            //            {
            //                LOG(Main, 1) << "i erase myFirstTarget " << (*it)->getId() << endl;
            //                firstTribes[myFirstTribesIndex[i]].erase(*it);
            //            }
            if (self()->getMotionlessObject()->getId() == (*it)->getId())
            {
                LOG(Main, 1) << "i erase myFirstTarget " << (*it)->getId() << endl;
                firstTribes[myFirstTribesIndex[i]].erase(*it);
            }

        }
    }
    LOG(Main, 1) << "End Of updateMyFirstTribe" << endl;
}

bool PoliceForceAgent::dontShasWhenIAmNotReachable()
{
    if (self()->getRepresentiveNodeIndex() == -1)
    {
        LOG(Main, 1) << "dontShasWhenIAmNotReachable()" << endl;
        Polygon* polygon = &self()->getMotionlessObject()->getShape();
        Point middle = polygon->getMidPoint();
        int random = rand() % polygon->size();
        Point p2 = polygon->getVertex(random);
        Point randomPoint = Segment(middle, p2).getMiddlePoint();
        //        int randomNeighbour = rand() % self()->getMotionlessObject()->getNeighbours().size();
        vector<int> path;
        path.push_back(self()->getMotionlessObject()->getId());
        //        path.push_back(world->motionlessObjects[self()->getMotionlessObject()->getNeighbours()[randomNeighbour]]->getId());
        command->move(path, randomPoint.getX(), randomPoint.getY());
        return true;
    }
    else
        return false;
}

void PoliceForceAgent::canGoRefuge()
{
    canIGoRefuge = false;
    for (int i = 0; i < world->refuges.size(); i++)
    {
        LOG(Main, 1) << "isReachable between me and refuge " << world->refuges[i]->getId() << " is " << worldGraph->isReachable(self()->getRepresentiveNodeIndex(), world->refuges[i]->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED) << endl;
        if (worldGraph->isReachable(self()->getRepresentiveNodeIndex(), world->refuges[i]->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED))
        {
            canIGoRefuge = true;
            break;
        }
    }
}

bool PoliceForceAgent::needEscape()
{
    if (self()->getDamage() / self()->getHp() > .02 || self()->getDamage() > 90 || (self()->getHp() < 3500 && self()->getDamage() > 0))
    {
        LOG(Main, 1) << "need escape" << endl;
        return true;
    }
    return false;
}

bool PoliceForceAgent::escape()
{
    if (needEscape())
    {
        if (world->refuges.size() > 0)
        {
            moveToRefuges();
            return true;
        }
    }
    return false;
}

bool PoliceForceAgent::dontShasWhenShasingInAPoint()
{
    Point newPosition = self()->getPos();
    if (lastPosition.distFromPoint(newPosition) < 1300 && self()->myLastCommand == CT_MOVE)
    {
        LOG(Main, 1) << "dontShasWhenShasingInAPoint" << endl;
        for (set<Blockade*>::iterator it = world->blockades.begin(); it != world->blockades.end(); it++)
        {
            Blockade* block = *it;
            if (block->getPos().distFromPoint(self()->getPos()) < maxClearDistance)
            {
                command->clear(*block);
                return true;
            }
            //            for(int i = 0 ; i < block->getShape().getVertices().size(); i++)
            //            {
            //                if ( block->getShape().getVertices()[i].distFromPoint(world->self->getPos()) < maxClearDistance )
            //                {
            //                    command->clear(*block);
            //                    return true;
            //                }
            //            }
        }
    }
    lastPosition = newPosition;
    return false;
}

bool PoliceForceAgent::dontShasWhenShasingInAMotionless()
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
        dontShas[0] = dontShas[1] = dontShas[2] = dontShas[3] = false;
        //		command->moveToPoint(self()->getMotionlessObject()->motionlessIndex, self()->getMotionlessObject()->getPos(), GM_WITHOUT_BLOCKADE);
        if (clear(self()->getMotionlessObject()))
            return true;
        command->moveToMotionless(self()->getMotionlessObject()->motionlessIndex, GM_WITHOUT_BLOCKADE);
        return true;
    }
    lastMotionless = self()->getMotionlessObject();
    return false;
}

void PoliceForceAgent::moveToRefuges()
{
    int minDist = MAX_INT;
    Refuge* ref;
    for (int i = 0; i < world->refuges.size(); i++)
    {
        int dist = worldGraph->getDistance(self()->getWithoutBlockadeRepresentiveNodeIndex(), world->refuges[i]->getWithoutBlockadeRepresentiveNodeIndex(), GM_WITHOUT_BLOCKADE);
        if (dist < minDist)
        {
            minDist = dist;
            ref = world->refuges[i];
        }
    }
    if (minDist != MAX_INT)
    {
        if (clear((MotionlessObject*) ref))
            return;
        command->moveToMotionless(ref->motionlessIndex, GM_WITHOUT_BLOCKADE);
    }
}

PoliceForce* PoliceForceAgent::self()
{
    return (PoliceForce*) world->self;
}

bool PoliceForceAgent::isNeighbor(Building* a, Building* b)
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

void PoliceForceAgent::dfs(BuildingBlock* buildingBlock, int color, Building* building)
{
    buildingBlock->buildings.push_back(building);
    building->blockNum = color;

    //    for (int i = 0; i < world->buildings.size(); i++)
    //    {
    //        if (isCloserThan(world->buildings[i], building) &&
    //                world->buildings[i]->blockNum == -1 &&
    //                isNeighbor(building, world->buildings[i]))
    //        {
    //            dfs(buildingBlock, color, world->buildings[i]);
    //        }
    //    }
    for (int i = 0; i < building->closerThan50B.size(); i++)
    {
        if (building->closerThan50B[i]->blockNum == -1 &&
                isNeighbor(building, building->closerThan50B[i]))
        {
            dfs(buildingBlock, color, building->closerThan50B[i]);
        }
    }
}

bool PoliceForceAgent::saveCivilians()
{
    for (int i = 0; i < world->humans.size(); i++)
    {
        Human* human = world->humans[i];
        if (worldGraph->getDistance(self()->getRepresentiveNodeIndex(), world->humans[i]->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED) <= 150000)
        {
            if (!worldGraph->isReachable(human->getRepresentiveNodeIndex(), self()->getRepresentiveNodeIndex(), GM_DEFAULT_BLOCKED) && human->getMotionlessObject()->isBuilding())
            {
                if (clear(human->getMotionlessObject()))
                    return true;
                command->moveToMotionless(human->getMotionlessObject()->motionlessIndex, GM_WITHOUT_BLOCKADE);
                return true;
            }
        }
    }
    return false;
}

void PoliceForceAgent::savePolices()
{
    LOG(Main, 1) << "start savePolices" << endl;
    std::set<PoliceForce*> newHelpNeededPolices;
    for (int i = 0; i < world->policeForces.size(); i++)
    {
        if (world->policeForces[i]->getBuriedness() > 0 && helpNeededPolices.find(world->policeForces[i]) == helpNeededPolices.end())
        {
            LOG(Main, 1) << world->policeForces[i]->getId() << " added " << endl;
            helpNeededPolices.insert(world->policeForces[i]);
            newHelpNeededPolices.insert(world->policeForces[i]);
        }
        else if (world->policeForces[i]->getBuriedness() == 0 && helpNeededPolices.find(world->policeForces[i]) != helpNeededPolices.end())
        {
            helpNeededPolices.erase(world->policeForces[i]);
        }
    }
    LOG(Main, 1) << "before for" << endl;
    for (set<PoliceForce*>::iterator it = newHelpNeededPolices.begin(); it != newHelpNeededPolices.end(); it++)
    {
        PoliceForce* wantHelp = *it;
        int minDist = MAX_INT;
        PoliceForce* minPF = NULL;
        for (int i = 0; i < world->policeForces.size(); i++)
        {
            if (helpNeededPolices.find(world->policeForces[i]) == helpNeededPolices.end())
            {
                LOG(Main, 1) << "in if " << world->policeForces[i]->getId() << endl;
                int repNode = world->policeForces[i]->getRepresentiveNodeIndex();
                if (world->policeForces[i]->policeForceIndex == self()->policeForceIndex)
                    repNode = lastMotionless->getRepresentiveNodeIndex();
                int dist = worldGraph->getAirDistance(repNode, wantHelp->getRepresentiveNodeIndex());
                LOG(Main, 1) << "dist from " << wantHelp->getId() << " is " << dist << endl;
                if (dist < minDist)
                {
                    minDist = dist;
                    minPF = world->policeForces[i];
                }
            }
        }
        LOG(Main, 1) << "afetr second for" << endl;
        if (minPF != NULL)
        {
            LOG(Main, 1) << "minPf to " << wantHelp->getId() << " is " << minPF->getId() << endl;
        }
        if (minPF != NULL && self()->policeForceIndex == minPF->policeForceIndex)
        {
            LOG(Main, 1) << "was me" << endl;
            set<MotionlessObject*> newVector;
            newVector.insert(wantHelp->getMotionlessObject());
            firstTribes.push_back(newVector);
            myFirstTribesIndex.push_back(firstTribes.size() - 1);
        }
    }
}

void PoliceForceAgent::createBuildingBlock()
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

bool PoliceForceAgent::isHere(vector<Road*> a, int b)
{
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i]->getId() == b)
            return true;
    }
    return false;
}

bool PoliceForceAgent::isCloserThan(Building* a, Building* b)
{
    for (int i = 0; i < a->closerThan50B.size(); i++)
    {
        if (a->closerThan50B[i]->getId() == b->getId())
        {
            return true;
        }
    }
    return false;
}


