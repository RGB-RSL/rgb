/* 
 * File:   WorldGraph.cpp
 * Author: Eqbal Sarjami
 * 
 * Created on November, 21, 2010
 */

#include "WorldGraph.h"
#include "Config.h"
#include "../Utilities/Geometry/GeometryFunctions.h"
#include <queue>
#include <algorithm>

#define LOGLEVEL 3

using namespace std;
using namespace Types;
using namespace Geometry;

WorldGraph::WorldGraph(WorldModel* world)
{
	this->world = world;
}

WorldGraph::~WorldGraph()
{
}

vector<Node*>& WorldGraph::getNodes()
{
    return nodes;
}

vector<Edge*>& WorldGraph::getEdges()
{
    return edges;
}

void WorldGraph::init()
{
    if(world->self->isPoliceForce())
        AGENT_RADIUS = 500;
    else
        AGENT_RADIUS = 500;
	createGraph();
	//        **** Logs of creating graph ****
	//        for(int i = 0; i < nodes.size(); i++)
	//        {
	//            LOG(Main, 1) << "Node: " << i << endl;
	//            LOG(Main, 1) << "\tfirst: " << world->motionlessObjects[nodes[i]->getFirstMotionlessIndex()]->getId() << endl;
	//            LOG(Main, 1) << "\tsecond: " << world->motionlessObjects[nodes[i]->getSecondMotionlessIndex()]->getId() << endl;
	//            LOG(Main, 1) << "\tNeighbours: " << endl;
	//            for(int j = 0; j < nodes[i]->getNeighbours().size(); j++)
	//            {
	//                LOG(Main, 1) << "\t\t" << nodes[i]->getNeighbours()[j] << endl;
	//            }
	//        }
	//        for(int i = 0; i < edges.size(); i++)
	//        {
	//            LOG(Main, 1) << "Edge: " << i << endl;
	//            LOG(Main, 1) << "\tfirstNode: " << edges[i]->getFirstNodeIndex() << endl;
	//            LOG(Main, 1) << "\tsecondNode: " << edges[i]->getSecondNodeIndex() << endl;
	//        }
	initRepresentiveNodes();
}

void WorldGraph::update()
{
	setDistanceFindingNodesAndSelfNodeIndexes();
	applyDFS();
	if(world->self->isFireBrigade())
	{
		if(world->teamConfig["MoveAndWorldGraphSettings"]["FireBrigadesDistanceFinding"]["UseWithoutBlockade"].asBool())
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() != 2)
			{
				applyWithoutBlockadeBFS();
			}
			applyWithoutBlockadeDijkstra();
		}
		if(world->teamConfig["MoveAndWorldGraphSettings"]["FireBrigadesDistanceFinding"]["UseDeafaultBlocked"].asBool())
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() != 2)
			{
				applyDefaultBlockedBFS();
			}
			applyDefaultBlockedDijkstra();
		}
		if(world->teamConfig["MoveAndWorldGraphSettings"]["FireBrigadesDistanceFinding"]["UseDeafaultNotBlocked"].asBool())
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() != 2)
			{
				applyDefaultNotBlockedBFS();
			}
			applyDefaultNotBlockedDijkstra();
		}
	}
	else if(world->self->isAmbulanceTeam())
	{
		if(world->teamConfig["MoveAndWorldGraphSettings"]["AmbulanceTeamsDistanceFinding"]["UseWithoutBlockade"].asBool())
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() != 2)
			{
				applyWithoutBlockadeBFS();
			}
			applyWithoutBlockadeDijkstra();
		}
		if(world->teamConfig["MoveAndWorldGraphSettings"]["AmbulanceTeamsDistanceFinding"]["UseDeafaultBlocked"].asBool())
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() != 2)
			{
				applyDefaultBlockedBFS();
			}
			applyDefaultBlockedDijkstra();
		}
		if(world->teamConfig["MoveAndWorldGraphSettings"]["AmbulanceTeamsDistanceFinding"]["UseDeafaultNotBlocked"].asBool())
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() != 2)
			{
				applyDefaultNotBlockedBFS();
			}
			applyDefaultNotBlockedDijkstra();
		}
	}
	else if(world->self->isPoliceForce())
	{
		if(world->teamConfig["MoveAndWorldGraphSettings"]["PoliceForcesDistanceFinding"]["UseWithoutBlockade"].asBool())
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() != 2)
			{
				applyWithoutBlockadeBFS();
			}
			applyWithoutBlockadeDijkstra();
		}
		if(world->teamConfig["MoveAndWorldGraphSettings"]["PoliceForcesDistanceFinding"]["UseDeafaultBlocked"].asBool())
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() != 2)
			{
				applyDefaultBlockedBFS();
			}
			applyDefaultBlockedDijkstra();
		}
		if(world->teamConfig["MoveAndWorldGraphSettings"]["PoliceForcesDistanceFinding"]["UseDeafaultNotBlocked"].asBool())
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() != 2)
			{
				applyDefaultNotBlockedBFS();
			}
			applyDefaultNotBlockedDijkstra();
		}
	}
	LOG(Main, 1) << "WorldGraph updated" << endl;
}

//node1Index must be the representive node of self object
vector<int> WorldGraph::getPath(int node1Index, int node2Index, GraphMode graphMode)
{
	vector<int> path;
	if(node1Index == -1 || node2Index == -1 || !isReachable(node1Index, node2Index, graphMode))
	{
		return path;
	}
	switch(graphMode)
	{
		case GM_DEFAULT_BLOCKED:
		{
			if(node1Index == withBlockadeSelfRepresentingNodeIndex && defaultBlockedDijkstraDistanceTable.size() > 0)
			{
				recursiveDijkstraPathFind(defaultBlockedDijkstraDistanceTable, path, node2Index);
			}
			return path;
			break;
		}
		case GM_DEFAULT_NOT_BLOCKED:
		{
			if(node1Index == withBlockadeSelfRepresentingNodeIndex && defaultNotBlockedDijkstraDistanceTable.size() > 0)
			{
				recursiveDijkstraPathFind(defaultNotBlockedDijkstraDistanceTable, path, node2Index);
			}
			return path;
			break;
		}
		case GM_WITHOUT_BLOCKADE:
		{
			if(node1Index == withoutBlockadeSelfRepresentingNodeIndex && withoutBlockadeDijkstraDistanceTable.size() > 0)
			{
				recursiveDijkstraPathFind(withoutBlockadeDijkstraDistanceTable, path, node2Index);
			}
			return path;
			break;
		}
	}
}

//node1Index must be in the finding distance humans or motionless representive nodes
int WorldGraph::getDistance(int node1Index, int node2Index, GraphMode graphMode)
{
	if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() == 2)
	{
		return getAirDistance(node1Index, node2Index);
	}
	if(node1Index == -1 || node2Index == -1 || !isReachable(node1Index, node2Index, graphMode))
	{
		return INF;
	}
	switch(graphMode)
	{
		case GM_DEFAULT_BLOCKED:
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() == 1 && defaultBlockedDistanceTable[node1Index].size() == 0)
			{
				return getAirDistance(node1Index, node2Index);
			}
			return defaultBlockedDistanceTable[node1Index][node2Index].distance;
			break;
		}
		case GM_DEFAULT_NOT_BLOCKED:
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() == 1 && defaultNotBlockedDistanceTable[node1Index].size() == 0)
			{
				return getAirDistance(node1Index, node2Index);
			}
			return defaultNotBlockedDistanceTable[node1Index][node2Index].distance;
			break;
		}
		case GM_WITHOUT_BLOCKADE:
		{
			if(world->teamConfig["MoveAndWorldGraphSettings"]["GlobalSettings"]["UseAirDistances"].asInt() == 1 && withoutBlockadeDistanceTable[node1Index].size() == 0)
			{
				return getAirDistance(node1Index, node2Index);
			}
			return withoutBlockadeDistanceTable[node1Index][node2Index].distance;
			break;
		}
	}
}

bool WorldGraph::isReachable(int node1Index, int node2Index, GraphMode graphMode)
{
	if(node1Index == -1 || node2Index == -1)
	{
		return false;
	}
	switch(graphMode)
	{
		case GM_DEFAULT_BLOCKED:
		{
			return defaultBlockedModeComponents[node1Index] == defaultBlockedModeComponents[node2Index];
			break;
		}
		case GM_DEFAULT_NOT_BLOCKED:
		{
			return defaultNotBlockedModeComponents[node1Index] == defaultNotBlockedModeComponents[node2Index];
			break;
		}
		case GM_WITHOUT_BLOCKADE:
		{
			return true;
			break;
		}
	}
}

vector<int> WorldGraph::getIDPath(vector<int>& nodePath)
{
	vector<int> idPath;
	if(nodePath.size() < 2)
		return idPath;
	for(int i = 1; i < (int)nodePath.size(); i++)
	{
		if(nodes[nodePath[i - 1]]->getFirstMotionlessIndex() == nodes[nodePath[i]]->getFirstMotionlessIndex() || nodes[nodePath[i - 1]]->getFirstMotionlessIndex() == nodes[nodePath[i]]->getSecondMotionlessIndex())
		{
			idPath.push_back(world->motionlessObjects[nodes[nodePath[i - 1]]->getFirstMotionlessIndex()]->getId());
		}
		else
		{
			idPath.push_back(world->motionlessObjects[nodes[nodePath[i - 1]]->getSecondMotionlessIndex()]->getId());
		}
	}
	return idPath;
}

void WorldGraph::addDistanceFindingMotiolessIndex(int motionlessIndex)
{
	distanceFindingMotionlessIndexes.push_back(motionlessIndex);
}

void WorldGraph::addDistanceFindingHumanIndex(int humanIndex)
{
	distanceFindingHumanIndexes.push_back(humanIndex);
}

int WorldGraph::getAirDistance(int node1Index, int node2Index)
{
    if(node1Index == -1 || node2Index == -1)
        return INF;
    return (int)distanceBetweenPoints(nodes[node1Index]->getRepresentivePoint(), nodes[node2Index]->getRepresentivePoint());
}

void WorldGraph::createGraph()
{
	//Creating Nodes
	int numOfNodes = 0;
	for(int i = 0; i < (int)world->motionlessObjects.size(); i++)
	{
		int firstMotionlessIndex = i;
		for(int j = 0; j < (int)world->motionlessObjects[firstMotionlessIndex]->getIDs().size(); j++)
		{
			int secondMotionlessIndex = world->motionlessObjects[firstMotionlessIndex]->getIDs()[j];
			if(secondMotionlessIndex != -1 && secondMotionlessIndex > firstMotionlessIndex)
			{
				Node *newNode = new Node(firstMotionlessIndex, secondMotionlessIndex);
				Point p = Segment(world->motionlessObjects[firstMotionlessIndex]->getShape().getVertex(j), world->motionlessObjects[firstMotionlessIndex]->getShape().getVertex((j + 1) % world->motionlessObjects[firstMotionlessIndex]->getIDs().size())).getMiddlePoint();
				newNode->setRepresentivePoint(p);
				newNode->setEdgeIndexInFirstMotionless(j);
				for(int k = 0; k < (int)world->motionlessObjects[secondMotionlessIndex]->getIDs().size(); k++)
				{
					if(world->motionlessObjects[secondMotionlessIndex]->getIDs()[k] == i)
					{
						newNode->setEdgeIndexInSecondMotionless(k);
						break;
					}
				}
				world->motionlessObjects[firstMotionlessIndex]->addRelativeNodeIndex(numOfNodes);
				world->motionlessObjects[secondMotionlessIndex]->addRelativeNodeIndex(numOfNodes);
				newNode->nodeIndex = numOfNodes++;
				nodes.push_back(newNode);
			}
		}
	}
	LOG(Main, 1) << "WorldGraph has " << numOfNodes << " nodes." << endl;

	//Resizing distance tables
	defaultBlockedModeComponents.resize(numOfNodes);
	defaultNotBlockedModeComponents.resize(numOfNodes);
	defaultBlockedDistanceTable.resize(numOfNodes);
	defaultNotBlockedDistanceTable.resize(numOfNodes);
	withoutBlockadeDistanceTable.resize(numOfNodes);
	heap.init(numOfNodes);

	//Creating Edges
	int numOfEdges = 0;
	for(int i = 0; i < (int)world->motionlessObjects.size(); i++)
	{
		//        LOG(Main, 1) << "Motionless id: " << world->motionlessObjects[i]->getId() << endl;
		//        LOG(Main, 1) << "\tnumber of relative nodes: " << world->motionlessObjects[i]->getRelativeNodesIndexes().size() << endl;
		for(int j = 0; j < (int)world->motionlessObjects[i]->getRelativeNodesIndexes().size(); j++)
		{
			//            LOG(Main, 1) << "\t\t" << world->motionlessObjects[i]->getRelativeNodesIndexes()[j] << endl;
			for(int k = j + 1; k < (int)world->motionlessObjects[i]->getRelativeNodesIndexes().size(); k++)
			{
				int firstNodeIndex = world->motionlessObjects[i]->getRelativeNodesIndexes()[j];
				int secondNodeIndex = world->motionlessObjects[i]->getRelativeNodesIndexes()[k];
				Edge *newEdge = new Edge(firstNodeIndex, secondNodeIndex);
				newEdge->setLength(nodes[firstNodeIndex]->getRepresentivePoint().distFromPoint(nodes[secondNodeIndex]->getRepresentivePoint()));
				nodes[firstNodeIndex]->addNeighbour(secondNodeIndex);
				nodes[secondNodeIndex]->addNeighbour(firstNodeIndex);
				nodes[firstNodeIndex]->addEdgeIndex(numOfEdges);
				nodes[secondNodeIndex]->addEdgeIndex(numOfEdges);
				world->motionlessObjects[i]->addRelativeEdgeIndex(numOfEdges);
				newEdge->setMotionlessIndex(i);
				newEdge->edgeIndex = numOfEdges++;
				edges.push_back(newEdge);
			}
		}
	}
	LOG(Main, 1) << "WorldGraph has " << numOfEdges << " edges." << endl;
	LOG(Main, 1) << "Graph creation finished." << endl;
}

void WorldGraph::setDistanceFindingNodesAndSelfNodeIndexes()
{
	withBlockadeDistanceFindingNodesIndexes.clear();
	withoutBlockadeDistanceFindingNodesIndexes.clear();
	for(int i = 0; i < (int)distanceFindingHumanIndexes.size(); i++)
	{
		int nodeIndex = world->humans[distanceFindingHumanIndexes[i]]->getRepresentiveNodeIndex();
		if(nodeIndex != -1)
		{
			withBlockadeDistanceFindingNodesIndexes.push_back(nodeIndex);
		}
		nodeIndex = world->humans[distanceFindingHumanIndexes[i]]->getWithoutBlockadeRepresentiveNodeIndex();
		withoutBlockadeDistanceFindingNodesIndexes.push_back(nodeIndex);
	}
	for(int i = 0; i < (int)distanceFindingMotionlessIndexes.size(); i++)
	{
		int nodeIndex = world->motionlessObjects[distanceFindingMotionlessIndexes[i]]->getRepresentiveNodeIndex();
		if(nodeIndex != -1)
		{
			withBlockadeDistanceFindingNodesIndexes.push_back(nodeIndex);
		}
		nodeIndex = world->motionlessObjects[distanceFindingMotionlessIndexes[i]]->getWithoutBlockadeRepresentiveNodeIndex();
		withoutBlockadeDistanceFindingNodesIndexes.push_back(nodeIndex);
	}
	withBlockadeSelfRepresentingNodeIndex = ((Human *)world->self)->getRepresentiveNodeIndex();
	withoutBlockadeSelfRepresentingNodeIndex = ((Human *)world->self)->getWithoutBlockadeRepresentiveNodeIndex();
}

void WorldGraph::applyDFS()
{
	int numOfComps = 0;
	mark.clear();
	mark.resize(nodes.size(), false);
	for(int i = 0; i < (int)nodes.size(); i++)
	{
		if(!mark[i])
		{
			recursiveDefaultBlockedDFS(i, numOfComps);
			numOfComps++;
		}
	}
	mark.clear();
	numOfComps = 0;
	mark.resize(nodes.size(), false);
	for(int i = 0; i < (int)nodes.size(); i++)
	{
		if(!mark[i])
		{
			recursiveDefaultNotBlockedDFS(i, numOfComps);
			numOfComps++;
		}
	}
	mark.clear();
}

void WorldGraph::applyDefaultBlockedBFS()
{
	LOG(Main, 2) << "before Default Blocked BFS" << endl;
	queue<int> que;
	for(int i = 0; i < (int)defaultBlockedDistanceTable.size(); i++)
	{
		defaultBlockedDistanceTable[i].clear();
	}
	for(int i = 0; i < (int)withBlockadeDistanceFindingNodesIndexes.size(); i++)
	{
		int rootIndex = withBlockadeDistanceFindingNodesIndexes[i];
		if((int)defaultBlockedDistanceTable[rootIndex].size() > 0)
		{
			continue;
		}
		mark.clear();
		mark.resize(nodes.size(), false);
		defaultBlockedDistanceTable[rootIndex].resize(nodes.size());
		defaultBlockedDistanceTable[rootIndex][rootIndex].distance = 0;
		mark[rootIndex] = true;
		que.push(rootIndex);
		while(!que.empty())
		{
			int curNode = que.front();
			for(int j = 0; j < (int)nodes[curNode]->getNeighbours().size(); j++)
			{
				int neighbour = nodes[curNode]->getNeighbours()[j];
				int edge = nodes[curNode]->getEdgeIndexes()[j];
				if(!mark[neighbour] && edges[edge]->getPassingMode() == PM_PASSABLE)
				{
					defaultBlockedDistanceTable[rootIndex][neighbour].distance = defaultBlockedDistanceTable[rootIndex][curNode].distance + edges[edge]->getLength();
					defaultBlockedDistanceTable[rootIndex][neighbour].parentNodeIndex = curNode;
					mark[neighbour] = true;
					que.push(neighbour);
				}
			}
			que.pop();
		}
	}
	LOG(Main, 2) << "after Default Blocked BFS" << endl;
}

void WorldGraph::applyDefaultNotBlockedBFS()
{
	LOG(Main, 2) << "before Default Not Blocked BFS" << endl;
	queue<int> que;
	for(int i = 0; i < (int)defaultNotBlockedDistanceTable.size(); i++)
	{
		defaultNotBlockedDistanceTable[i].clear();
	}
	for(int i = 0; i < (int)withBlockadeDistanceFindingNodesIndexes.size(); i++)
	{
		int rootIndex = withBlockadeDistanceFindingNodesIndexes[i];
		if((int)defaultNotBlockedDistanceTable[rootIndex].size() > 0)
		{
			continue;
		}
		mark.clear();
		mark.resize(nodes.size(), false);
		defaultNotBlockedDistanceTable[rootIndex].resize(nodes.size());
		defaultNotBlockedDistanceTable[rootIndex][rootIndex].distance = 0;
		mark[rootIndex] = true;
		que.push(rootIndex);
		while(!que.empty())
		{
			int curNode = que.front();
			for(int j = 0; j < (int)nodes[curNode]->getNeighbours().size(); j++)
			{
				int neighbour = nodes[curNode]->getNeighbours()[j];
				int edge = nodes[curNode]->getEdgeIndexes()[j];
				if(!mark[neighbour] && (edges[edge]->getPassingMode() == PM_PASSABLE || edges[edge]->getPassingMode() == PM_UNKNOWN))
				{
					defaultNotBlockedDistanceTable[rootIndex][neighbour].distance = defaultNotBlockedDistanceTable[rootIndex][curNode].distance + edges[edge]->getLength();
					defaultNotBlockedDistanceTable[rootIndex][neighbour].parentNodeIndex = curNode;
					mark[neighbour] = true;
					que.push(neighbour);
				}
			}
			que.pop();
		}
	}
	LOG(Main, 2) << "after Default Not Blocked BFS" << endl;
}

void WorldGraph::applyWithoutBlockadeBFS()
{
	LOG(Main, 2) << "before Without Blockade BFS" << endl;
	queue<int> que;
	for(int i = 0; i < (int)withoutBlockadeDistanceTable.size(); i++)
	{
		withoutBlockadeDistanceTable[i].clear();
	}
	for(int i = 0; i < (int)withoutBlockadeDistanceFindingNodesIndexes.size(); i++)
	{
		int rootIndex = withoutBlockadeDistanceFindingNodesIndexes[i];
		if((int)withoutBlockadeDistanceTable[rootIndex].size() > 0)
		{
			continue;
		}
		mark.clear();
		mark.resize(nodes.size(), false);
		withoutBlockadeDistanceTable[rootIndex].resize(nodes.size());
		withoutBlockadeDistanceTable[rootIndex][rootIndex].distance = 0;
		mark[rootIndex] = true;
		que.push(rootIndex);
		while(!que.empty())
		{
			int curNode = que.front();
			for(int j = 0; j < (int)nodes[curNode]->getNeighbours().size(); j++)
			{
				int neighbour = nodes[curNode]->getNeighbours()[j];
				int edge = nodes[curNode]->getEdgeIndexes()[j];
				if(!mark[neighbour])
				{
					withoutBlockadeDistanceTable[rootIndex][neighbour].distance = withoutBlockadeDistanceTable[rootIndex][curNode].distance + edges[edge]->getLength();
					withoutBlockadeDistanceTable[rootIndex][neighbour].parentNodeIndex = curNode;
					mark[neighbour] = true;
					que.push(neighbour);
				}
			}
			que.pop();
		}
	}
	LOG(Main, 2) << "after Without Blockade BFS: " << endl;
}

void WorldGraph::applyDefaultBlockedDijkstra()
{
	heap.clear();
	int rootNode = withBlockadeSelfRepresentingNodeIndex;
	defaultBlockedDijkstraDistanceTable.clear();
	defaultBlockedDijkstraDistanceTable.resize(nodes.size());
	mark.clear();
	mark.resize(nodes.size(), false);
	for(int i = 0; i < (int)nodes.size(); i++)
	{
		heap.insert(i, INF);
	}
	defaultBlockedDijkstraDistanceTable[rootNode].distance = 0;
	heap.update(rootNode, 0);
	for(int i = 0; i < (int)nodes.size(); i++)
	{
		int curNode = heap.extractMin();
		if(defaultBlockedDijkstraDistanceTable[curNode].distance == INF)
		{
			break;
		}
		mark[curNode] = true;
		for(int j = 0; j < (int)nodes[curNode]->getNeighbours().size(); j++)
		{
			int neighbour = nodes[curNode]->getNeighbours()[j];
			int edge = nodes[curNode]->getEdgeIndexes()[j];
			if(!mark[neighbour] && edges[edge]->getPassingMode() == PM_PASSABLE && defaultBlockedDijkstraDistanceTable[curNode].distance + edges[edge]->getLength() < defaultBlockedDijkstraDistanceTable[neighbour].distance)
			{
				defaultBlockedDijkstraDistanceTable[neighbour].distance = defaultBlockedDijkstraDistanceTable[curNode].distance + edges[edge]->getLength();
				defaultBlockedDijkstraDistanceTable[neighbour].parentNodeIndex = curNode;
				heap.update(neighbour, defaultBlockedDijkstraDistanceTable[neighbour].distance);
			}
		}
	}
}

void WorldGraph::applyDefaultNotBlockedDijkstra()
{
	heap.clear();
	int rootNode = withBlockadeSelfRepresentingNodeIndex;
	defaultNotBlockedDijkstraDistanceTable.clear();
	defaultNotBlockedDijkstraDistanceTable.resize(nodes.size());
	mark.clear();
	mark.resize(nodes.size(), false);
	for(int i = 0; i < (int)nodes.size(); i++)
	{
		heap.insert(i, INF);
	}
	defaultNotBlockedDijkstraDistanceTable[rootNode].distance = 0;
	heap.update(rootNode, 0);
	for(int i = 0; i < (int)nodes.size(); i++)
	{
		int curNode = heap.extractMin();
		if(defaultNotBlockedDijkstraDistanceTable[curNode].distance == INF)
		{
			break;
		}
		mark[curNode] = true;
		for(int j = 0; j < (int)nodes[curNode]->getNeighbours().size(); j++)
		{
			int neighbour = nodes[curNode]->getNeighbours()[j];
			int edge = nodes[curNode]->getEdgeIndexes()[j];
			if(!mark[neighbour] && (edges[edge]->getPassingMode() == PM_PASSABLE || edges[edge]->getPassingMode() == PM_UNKNOWN) && defaultNotBlockedDijkstraDistanceTable[curNode].distance + edges[edge]->getLength() < defaultNotBlockedDijkstraDistanceTable[neighbour].distance)
			{
				defaultNotBlockedDijkstraDistanceTable[neighbour].distance = defaultNotBlockedDijkstraDistanceTable[curNode].distance + edges[edge]->getLength();
				defaultNotBlockedDijkstraDistanceTable[neighbour].parentNodeIndex = curNode;
				heap.update(neighbour, defaultNotBlockedDijkstraDistanceTable[neighbour].distance);
			}
		}
	}
}

void WorldGraph::applyWithoutBlockadeDijkstra()
{
	heap.clear();
	int rootNode = withoutBlockadeSelfRepresentingNodeIndex;
	withoutBlockadeDijkstraDistanceTable.clear();
	withoutBlockadeDijkstraDistanceTable.resize(nodes.size());
	mark.clear();
	mark.resize(nodes.size(), false);
	for(int i = 0; i < (int)nodes.size(); i++)
	{
		heap.insert(i, INF);
	}
	withoutBlockadeDijkstraDistanceTable[rootNode].distance = 0;
	heap.update(rootNode, 0);
	for(int i = 0; i < (int)nodes.size(); i++)
	{
		int curNode = heap.extractMin();
		if(withoutBlockadeDijkstraDistanceTable[curNode].distance == INF)
		{
			break;
		}
		mark[curNode] = true;
		for(int j = 0; j < (int)nodes[curNode]->getNeighbours().size(); j++)
		{
			int neighbour = nodes[curNode]->getNeighbours()[j];
			int edge = nodes[curNode]->getEdgeIndexes()[j];
			if(!mark[neighbour] && withoutBlockadeDijkstraDistanceTable[curNode].distance + edges[edge]->getLength() < withoutBlockadeDijkstraDistanceTable[neighbour].distance)
			{
				withoutBlockadeDijkstraDistanceTable[neighbour].distance = withoutBlockadeDijkstraDistanceTable[curNode].distance + edges[edge]->getLength();
				withoutBlockadeDijkstraDistanceTable[neighbour].parentNodeIndex = curNode;
				heap.update(neighbour, withoutBlockadeDijkstraDistanceTable[neighbour].distance);
			}
		}
	}
}

void WorldGraph::recursiveDefaultBlockedDFS(int nodeIndex, int comp)
{
	mark[nodeIndex] = true;
	defaultBlockedModeComponents[nodeIndex] = comp;
	for(int i = 0; i < (int)nodes[nodeIndex]->getNeighbours().size(); i++)
	{
		int neighbour = nodes[nodeIndex]->getNeighbours()[i];
		PassingMode pm = edges[nodes[nodeIndex]->getEdgeIndexes()[i]]->getPassingMode();
		if(!mark[neighbour] && pm == PM_PASSABLE)
		{
			recursiveDefaultBlockedDFS(neighbour, comp);
		}
	}
}

void WorldGraph::recursiveDefaultNotBlockedDFS(int nodeIndex, int comp)
{
	mark[nodeIndex] = true;
	defaultNotBlockedModeComponents[nodeIndex] = comp;
	for(int i = 0; i < (int)nodes[nodeIndex]->getNeighbours().size(); i++)
	{
		int neighbour = nodes[nodeIndex]->getNeighbours()[i];
		PassingMode pm = edges[nodes[nodeIndex]->getEdgeIndexes()[i]]->getPassingMode();
		if(!mark[neighbour] && (pm == PM_PASSABLE || pm == PM_UNKNOWN))
		{
			recursiveDefaultNotBlockedDFS(neighbour, comp);
		}
	}
}

void WorldGraph::recursiveDijkstraPathFind(std::vector<WorldGraph::DistanceTableEntry>& distanceTable, std::vector<int>& path, int nodeID)
{
	if(distanceTable[nodeID].parentNodeIndex != -1)
	{
		recursiveDijkstraPathFind(distanceTable, path, distanceTable[nodeID].parentNodeIndex);
	}
	path.push_back(nodeID);
}

WorldGraph::DistanceTableEntry::DistanceTableEntry(int dist, int parent)
{
	distance = dist;
	parentNodeIndex = parent;
}

int WorldGraph::UpdatableHeap::lChild(int id)
{
	return id * 2 + 1;
}

int WorldGraph::UpdatableHeap::rChild(int id)
{
	return id * 2 + 2;
}

int WorldGraph::UpdatableHeap::parent(int id)
{
	return (id - 1) / 2;
}

void WorldGraph::UpdatableHeap::bubbleUp(int id)
{
	if(id != ROOT && entries[id].value < entries[parent(id)].value)
	{
		places[entries[parent(id)].index] = id;
		places[entries[id].index] = parent(id);
		swap(entries[id], entries[parent(id)]);
		bubbleUp(parent(id));
	}
}

void WorldGraph::UpdatableHeap::bubbleDown(int id)
{
	int minId = id;
	int minVal = entries[id].value;
	if(lChild(id) < size && minVal > entries[lChild(id)].value)
	{
		minId = lChild(id);
		minVal = entries[lChild(id)].value;
	}
	if(rChild(id) < size && minVal > entries[rChild(id)].value)
	{
		minId = rChild(id);
		minVal = entries[rChild(id)].value;
	}
	if(minId != id)
	{
		places[entries[minId].index] = id;
		places[entries[id].index] = minId;
		swap(entries[id], entries[minId]);
		bubbleDown(minId);
	}
}

void WorldGraph::UpdatableHeap::init(int size)
{
	this->size = 0;
	places.resize(size, -1);
}

void WorldGraph::UpdatableHeap::insert(int index, int value)
{
	places[index] = size;
	Entry newEntry;
	newEntry.index = index;
	newEntry.value = value;
	entries.push_back(newEntry);
	bubbleUp(size);
	size++;
}

void WorldGraph::UpdatableHeap::update(int index, int newValue)
{
	int id = places[index];
	entries[id].value = newValue;
	if(entries[id].value < entries[parent(id)].value)
	{
		bubbleUp(id);
	}
	else
	{
		bubbleDown(id);
	}
}

int WorldGraph::UpdatableHeap::extractMin()
{
	if(size == 0)
	{
		return -1;
	}
	int tmp = entries[ROOT].index;
	entries[ROOT] = entries[size - 1];
	places[entries[ROOT].index] = ROOT;
	entries.pop_back();
	size--;
	bubbleDown(ROOT);
	return tmp;
}

bool WorldGraph::UpdatableHeap::empty()
{
	return size == 0;
}

void WorldGraph::UpdatableHeap::clear()
{
	size = 0;
	entries.clear();
	for(int i = 0; i < (int)places.size(); i++)
	{
		places[i] = -1;
	}
}