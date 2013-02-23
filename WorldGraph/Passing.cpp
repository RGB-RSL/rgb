/*
 * File:   Passing.cpp
 * Author: Eqbal Sarjami
 *
 * Created on March, 20 , 2011
 */

#include "WorldGraph.h"
#include "../Utilities/Geometry/GeometryFunctions.h"
#include "../Utilities/Debugger.h"
#include "Circle.h"
#include <cassert>
#include <stdlib.h>

#define LOGLEVEL 0

using namespace std;
using namespace Types;
using namespace Geometry;

void WorldGraph::updatePassingModesAndRepresentiveNodes(int motionLessIndex)
{
    MotionlessObject *mlo = world->motionlessObjects[motionLessIndex];
    if (mlo->isBuilding() || ((Road*) mlo)->getBlockadeIds().size() == 0)
    {
        mlo->setRepresentiveNodeIndex(mlo->getWithoutBlockadeRepresentiveNodeIndex());
        for (int i = 0; i < (int) mlo->getRelativeEdgesIndexes().size(); i++)
        {
            if (edges[mlo->getRelativeEdgesIndexes()[i]]->getPassingMode() != PM_PASSABLE)
            {
                edges[mlo->getRelativeEdgesIndexes()[i]]->setPassingMode(PM_PASSABLE);
                if (mlo->isRoad())
                {
                    ((Road*) mlo)->setHasToBeSent(world->getTime());
                }
            }
        }
        for (int i = 0; i < (int) mlo->getInnerHumansIndexes().size(); i++)
        {
            Human *human = world->humans[mlo->getInnerHumansIndexes()[i]];
            human->setRepresentiveNodeIndex(human->getWithoutBlockadeRepresentiveNodeIndex());
        }
        LOG(Main, 2) << "Motionless " << mlo->getId() << " is setted without blockade." << endl;
        return;
    }

    Road *r = (Road*) mlo;
    LOG(Main, 1) << "Road " << r->getId() << " is started isPassable." << endl;
    polygons.clear();
    pPoints.clear();
    addRoadAndBlockadesToPassingPoints(r);
    LOG(Main, 2) << "1" << endl;
    map<int, int> nodeToPPoint;
    for (int i = 0; i < (int) r->getRelativeNodesIndexes().size(); i++)
    {
        Node *node = nodes[r->getRelativeNodesIndexes()[i]];
        Segment s = polygons[0].getSegment((r->motionlessIndex == node->getFirstMotionlessIndex()) ? node->getEdgeIndexInFirstMotionless() : node->getEdgeIndexInSecondMotionless());
        nodeToPPoint[node->nodeIndex] = pPoints.size();
        pPoints.push_back(PassingPoint(s.getMiddlePoint(), 0));
    }
    LOG(Main, 2) << "2" << endl;
    vector<int> humanToPPoint;
    for (int i = 0; i < (int) r->getInnerHumansIndexes().size(); i++)
    {
        Human *human = world->humans[r->getInnerHumansIndexes()[i]];
        humanToPPoint.push_back(pPoints.size());
        pPoints.push_back(PassingPoint(human->getPos(), INF));
    }
    LOG(Main, 2) << "3" << endl;
    setAvailablePoints();
    LOG(Main, 2) << "4" << endl;
    createPassingGraph();
    LOG(Main, 2) << "5" << endl;
    findComponentsInPassingGraph();
    LOG(Main, 2) << "6" << endl;
    for (int i = 0; i < (int) r->getRelativeEdgesIndexes().size(); i++)
    {
        Edge *edge = edges[r->getRelativeEdgesIndexes()[i]];
        int pp1 = nodeToPPoint[edge->getFirstNodeIndex()];
        int pp2 = nodeToPPoint[edge->getSecondNodeIndex()];
        if (pPoints[pp1].isAv != false && pPoints[pp2].isAv != false && pComps[pPoints[pp1].pNodeIndex] == pComps[pPoints[pp2].pNodeIndex])
        {
            //			setColor(0);
            if (edge->getPassingMode() != PM_PASSABLE)
            {
                edge->setPassingMode(PM_PASSABLE);
                r->setHasToBeSent(world->getTime());
            }
        }
        else
        {
            //			setColor(4);
            if (edge->getPassingMode() != PM_NOT_PASSABLE)
            {
                edge->setPassingMode(PM_NOT_PASSABLE);
                r->setHasToBeSent(world->getTime());
            }
        }
        //		LOG(Main, 1) << Segment(nodes[edge->getFirstNodeIndex()]->getRepresentivePoint(), nodes[edge->getSecondNodeIndex()]->getRepresentivePoint()) << ' ';
    }
    for (int i = 0; i < (int) r->getInnerHumansIndexes().size(); i++)
    {
        Human *human = world->humans[r->getInnerHumansIndexes()[i]];
        if (!pPoints[humanToPPoint[i]].isAv)
        {
            human->setRepresentiveNodeIndex(-1);
        }
        else
        {
            double minD = 1e20, minNodeId = -1;
            for (int j = 0; j < (int) r->getRelativeNodesIndexes().size(); j++)
            {
                int nodeId = r->getRelativeNodesIndexes()[j];
                int pp = nodeToPPoint[nodeId];
                if (pPoints[pp].isAv && pComps[pPoints[humanToPPoint[i]].pNodeIndex] == pComps[pPoints[pp].pNodeIndex])
                {
                    double dist = distanceBetweenPoints(human->getPos(), nodes[nodeId]->getRepresentivePoint());
                    if (isLess(dist, minD))
                    {
                        minD = dist;
                        minNodeId = nodeId;
                    }
                    //					setColor(0);
                }
                else
                {
                    //					setColor(4);
                }
                //				LOG(Main, 1) << Segment(nodes[nodeId]->getRepresentivePoint(), human->getPos()) << ' ';
            }
            human->setRepresentiveNodeIndex(minNodeId);
        }
    }
    r->setRepresentiveNodeIndex(-1);
    for (int i = 0; i < (int) r->getRelativeNodesIndexes().size(); i++)
    {
        int pp = nodeToPPoint[r->getRelativeNodesIndexes()[i]];
        if (pPoints[pp].isAv)
        {
            r->setRepresentiveNodeIndex(r->getRelativeNodesIndexes()[i]);
            break;
        }
    }

    //	setColor(3);
    //	for(int i = 0; i < (int)r->getBlockades().size(); i++)
    //	{
    //		LOG(Main, 1) << r->getBlockades()[i]->getShape() << ' ';
    //	}
//    setColor(3);
//    LOG(Main, 1) << polygons[0] << ' ';
//    	setColor(1);
//    	for(int i = 1; i < (int)polygons.size(); i++)
//    	{
//    		LOG(Main, 1) << polygons[i] << ' ';
//    	}
//    	for(int i = 0; i < (int)pPoints.size(); i++)
//    	{
//    		if(pPoints[i].isAv)
//    		{
//    			setColor(0);
//    		}
//    		else
//    		{
//    			setColor(4);
//    		}
//    		LOG(Main, 1) << Circle(pPoints[i].p, 100) << ' ';
//    	}
//        LOG(Main, 1) << endl;
//    	setColor(2);
//    	for(int i = 0; i < numOfPNodes; i++)
//    	{
//    		for(int j = 0; j < (int)pEdges[i].size(); j++)
//    		{
//    			LOG(Main, 1) << Segment(pPoints[pNodeToPPoint[i]].p, pPoints[pNodeToPPoint[pEdges[i][j]]].p) << ' ';
//    		}
//    	}
//    	LOG(Main, 1) << endl;
    LOG(Main, 1) << "isPassable finished." << endl;
}

void WorldGraph::addRoadAndBlockadesToPassingPoints(Road* r)
{
    polygons.push_back(increasePolygon(r->getShape(), -AGENT_RADIUS));
    for (int i = 0; i < (int) r->getBlockades().size(); i++)
    {
        polygons.push_back(increasePolygon(r->getBlockades()[i]->getShape(), AGENT_RADIUS));
    }
    for (int i = 0; i < (int) polygons.size(); i++)
    {
        for (int j = 0; j < polygons[i].size(); j++)
        {
            pPoints.push_back(PassingPoint(polygons[i].getVertex(j), i));
        }
    }
}

void WorldGraph::setAvailablePoints()
{
    for (int i = 0; i < (int) pPoints.size(); i++)
    {
        for (int j = 0; j < (int) polygons.size(); j++)
        {
            if (pPoints[i].f != INF && pPoints[i].f != j && (((j == 0) ? 1 : 0) ^ polygons[j].isInPolygon(pPoints[i].p)))
            {
                //				LOG(Main, 1) << pPoints[i].p << ' ' << pPoints[i].f << ' ' << j << endl;
                pPoints[i].isAv = false;
                break;
            }
        }
    }
}

void WorldGraph::createPassingGraph()
{
    pNodeToPPoint.clear();
    pEdges.clear();
    numOfPNodes = 0;
    for (int i = 0; i < (int) pPoints.size(); i++)
    {
        if (pPoints[i].isAv)
        {
            pNodeToPPoint.push_back(i);
            vector<int> tmp;
            pEdges.push_back(tmp);
            pPoints[i].pNodeIndex = numOfPNodes;
            for (int j = 0; j < numOfPNodes; j++)
            {
                Segment s(pPoints[i].p, pPoints[pNodeToPPoint[j]].p);
                bool isGood = true;
                for (int k = 0; k < (int) polygons.size(); k++)
                {
                    if (((k == 0) ? 1 : 0) ^ polygons[k].isInPolygon(s.getMiddlePoint()))
                    {
                        isGood = false;
                        break;
                    }
                    for (int l = 0; l < polygons[k].size(); l++)
                    {
                        if (isOnSegment(polygons[k].getSegment(l), pPoints[i].p) || isOnSegment(polygons[k].getSegment(l), pPoints[pNodeToPPoint[j]].p))
                        {
                            continue;
                        }
                        if (isIntersect(polygons[k].getSegment(l), s))
                        {
                            isGood = false;
                            break;
                        }
                    }
                    if (!isGood)
                    {
                        break;
                    }
                }
                if (isGood)
                {
                    pEdges[numOfPNodes].push_back(j);
                    pEdges[j].push_back(numOfPNodes);
                }
            }
            numOfPNodes++;
        }
    }
}

void WorldGraph::findComponentsInPassingGraph()
{
    pComps.clear();
    mark.clear();
    pComps.resize(numOfPNodes, -1);
    mark.resize(numOfPNodes, false);
    int numOfComps = 0;
    for (int i = 0; i < numOfPNodes; i++)
    {
        if (!mark[i])
        {
            recursiveDFSInPassingGraph(i, numOfComps);
            numOfComps++;
        }
    }
}

void WorldGraph::recursiveDFSInPassingGraph(int pNodeIndex, int comp)
{
    mark[pNodeIndex] = true;
    pComps[pNodeIndex] = comp;
    for (int i = 0; i < (int) pEdges[pNodeIndex].size(); i++)
    {
        if (!mark[pEdges[pNodeIndex][i]])
        {
            recursiveDFSInPassingGraph(pEdges[pNodeIndex][i], comp);
        }
    }
}

void WorldGraph::updateWithoutBlockadeRepresentiveNode(int humanIndex)
{
    Human *human = world->humans[humanIndex];
    MotionlessObject *pos = human->getMotionlessObject();
    double minD = 1e20, minNodeId = -1;
    for (int i = 0; i < (int) pos->getRelativeNodesIndexes().size(); i++)
    {
        int nodeId = pos->getRelativeNodesIndexes()[i];
        double dist = distanceBetweenPoints(human->getPos(), nodes[nodeId]->getRepresentivePoint());
        if (isLess(dist, minD))
        {
            minD = dist;
            minNodeId = nodeId;
        }
    }
    human->setWithoutBlockadeRepresentiveNodeIndex(minNodeId);
}

int WorldGraph::getDynamicRepresentiveNode(int motionlessIndex, Point p)
{
    //it has to be corrected
    return world->motionlessObjects[motionlessIndex]->getRepresentiveNodeIndex();
}

void WorldGraph::initRepresentiveNodes()
{
    for (int i = 0; i < (int) world->motionlessObjects.size(); i++)
    {
        world->motionlessObjects[i]->setRepresentiveNodeIndex(world->motionlessObjects[i]->getRelativeNodesIndexes()[0]);
        world->motionlessObjects[i]->setWithoutBlockadeRepresentiveNodeIndex(world->motionlessObjects[i]->getRelativeNodesIndexes()[0]);
    }
    for (int i = 0; i < (int) world->humans.size(); i++)
    {
        world->humans[i]->setRepresentiveNodeIndex(world->humans[i]->getMotionlessObject()->getRepresentiveNodeIndex());
        world->humans[i]->setWithoutBlockadeRepresentiveNodeIndex(world->humans[i]->getMotionlessObject()->getWithoutBlockadeRepresentiveNodeIndex());
    }
}

WorldGraph::PassingPoint::PassingPoint()
{
    isAv = true;
}

WorldGraph::PassingPoint::PassingPoint(Geometry::Point p, int f)
{
    this->p = p;
    this->f = f;
    isAv = true;
}