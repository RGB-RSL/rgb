/* 
 * File:   WorldGraph.h
 * Author: Eqbal Sarjami
 *
 * Created on November, 21, 2010
 */

#ifndef _WORLDGRAPH_H
#define	_WORLDGRAPH_H

#include "../WorldModel/WorldModel.h"
#include "../Utilities/Debugger.h"
#include "../Utilities/Geometry/Segment.h"
#include "../WorldModel/Objects/Node.h"
#include "../WorldModel/Objects/Edge.h"
#include <cstring>
#define LOGLEVEL 0;

class WorldGraph
{
public:
	WorldGraph(WorldModel*);
	virtual ~WorldGraph();

	std::vector<Node*>& getNodes();
	std::vector<Edge*>& getEdges();

	void init();
	void updatePassingModesAndRepresentiveNodes(int motionLessIndex);
	void updateWithoutBlockadeRepresentiveNode(int humanIndex);
	void update();
	std::vector<int> getPath(int node1Index, int node2Index, Types::GraphMode graphMode);
	int getDistance(int node1Index, int node2Index, Types::GraphMode graphMode);
	bool isReachable(int node1Index, int node2Index, Types::GraphMode graphMode);
	std::vector<int> getIDPath(std::vector<int>& nodePath);
	int getDynamicRepresentiveNode(int motionlessIndex, Geometry::Point p);
	void addDistanceFindingMotiolessIndex(int motionlessIndex);
	void addDistanceFindingHumanIndex(int humanIndex);
	int getAirDistance(int node1Index, int node2Index);

private:
	const static int INF = (1u << 30) - 1;

	struct DistanceTableEntry
	{
		int distance;
		int parentNodeIndex;
		DistanceTableEntry(int dist = INF, int parent = -1);
	};

	class UpdatableHeap
	{
	public:

		struct Entry
		{
			int value;
			int index;
		};
		void init(int size);
		void insert(int index, int value);
		void update(int index, int newValue);
		int extractMin();
		bool empty();
		void clear();
	private:
		const static int ROOT = 0;
		std::vector<int> places;
		std::vector<Entry> entries;
		int size;
		int lChild(int id);
		int rChild(int id);
		int parent(int id);
		void bubbleUp(int id);
		void bubbleDown(int id);
	} heap;

	void createGraph();
	void setDistanceFindingNodesAndSelfNodeIndexes();
	void applyDFS();
	void applyDefaultBlockedBFS();
	void applyDefaultNotBlockedBFS();
	void applyWithoutBlockadeBFS();
	void applyDefaultBlockedDijkstra();
	void applyDefaultNotBlockedDijkstra();
	void applyWithoutBlockadeDijkstra();
	void recursiveDefaultBlockedDFS(int, int);
	void recursiveDefaultNotBlockedDFS(int, int);
	void recursiveDijkstraPathFind(std::vector<DistanceTableEntry>&, std::vector<int>&, int);
	void initRepresentiveNodes();
	WorldModel *world;
	std::vector<Node*> nodes;
	std::vector<Edge*> edges;
	std::vector<bool> mark;
	std::vector<int> defaultBlockedModeComponents;
	std::vector<int> defaultNotBlockedModeComponents;
	std::vector<int> withBlockadeDistanceFindingNodesIndexes;
	std::vector<int> withoutBlockadeDistanceFindingNodesIndexes;
	int withBlockadeSelfRepresentingNodeIndex;
	int withoutBlockadeSelfRepresentingNodeIndex;
	std::vector<int> distanceFindingMotionlessIndexes;
	std::vector<int> distanceFindingHumanIndexes;
	std::vector<std::vector<DistanceTableEntry> > defaultBlockedDistanceTable;
	std::vector<std::vector<DistanceTableEntry> > defaultNotBlockedDistanceTable;
	std::vector<std::vector<DistanceTableEntry> > withoutBlockadeDistanceTable;
	std::vector<DistanceTableEntry> defaultBlockedDijkstraDistanceTable;
	std::vector<DistanceTableEntry> defaultNotBlockedDijkstraDistanceTable;
	std::vector<DistanceTableEntry> withoutBlockadeDijkstraDistanceTable;

	//Passing

	struct PassingPoint
	{
		Geometry::Point p;
		int f;
		bool isAv;
		int pNodeIndex;
		PassingPoint();
		PassingPoint(Geometry::Point, int);
	};

	int AGENT_RADIUS;
	std::vector<Geometry::Polygon> polygons;
	std::vector<PassingPoint> pPoints;
	std::vector<std::vector<int> > pEdges;
	std::vector<int> pComps;
	std::vector<int> pNodeToPPoint;
	int numOfPNodes;
	void addRoadAndBlockadesToPassingPoints(Road*);
	void setAvailablePoints();
	void createPassingGraph();
	void findComponentsInPassingGraph();
	void recursiveDFSInPassingGraph(int, int);
};

#endif	/* _WORLDGRAPH_H */
