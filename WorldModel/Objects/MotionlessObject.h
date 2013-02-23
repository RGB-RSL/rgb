/*
 * MotionlessObject.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#ifndef __MOTIONLESS_OBJECT_
#define __MOTIONLESS_OBJECT_

#include "RCRObject.h"
#include "../../Utilities/Geometry/Polygon.h"
#include <set>
#include <cmath>
class Node;
class GraphEdge;

class MotionlessObject: public RCRObject
{
public:
	MotionlessObject(unsigned int Id);
	MotionlessObject();
	virtual ~MotionlessObject();
	//    std::vector<Edge> getEdges() const;
	void addEdge(int a);
	void setProperties(std::string type, int value);
	Geometry::Polygon& getShape();
	std::vector<int> & getIDs();
	void setID(int, int);
	std::vector<int>& getRelativeNodesIndexes();
	void addRelativeNodeIndex(int);
	std::vector<int>& getRelativeEdgesIndexes();
	void addRelativeEdgeIndex(int);
	std::vector<int>& getNeighbours();
	void addNeighbour(int);
	std::vector<int>& getInnerHumansIndexes();
	void addInnerHumanIndex(int);
	void clearInnerHumans();
	int getRepresentiveNodeIndex();
	int getWithoutBlockadeRepresentiveNodeIndex();
	void setRepresentiveNodeIndex(int);
	void setWithoutBlockadeRepresentiveNodeIndex(int);
	int motionlessIndex;
        bool iHaveBeenHere;
    std::vector<MotionlessObject *> mCloserThan30B;
    std::vector<MotionlessObject *> mCloserThan40B;
    std::vector<MotionlessObject *> mCloserThan50B;

private:
	Geometry::Polygon shape;
	std::vector <int> ids;
	std::vector<int> relativeNodesIndexes;
	std::vector<int> relativeEdgesIndexes;
	std::vector<int> neighbours;
	std::vector<int> innerHumansIndexes;
	int representiveNodeIndex;
	int withoutBlockadeRepresentiveNodeIndex;
};

#endif