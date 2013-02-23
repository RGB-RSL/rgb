/* 
 * File:   Node.h
 * Author: Eqbal Sarjami
 *
 * Created on November 28, 2010, 1:37 PM
 */

#ifndef _NODE_H
#define	_NODE_H

#include "../../Utilities/Geometry/Point.h"

class Node
{
public:
	Node(int, int);
	~Node();
	int getFirstMotionlessIndex();
	int getSecondMotionlessIndex();
	std::vector<int>& getEdgeIndexes();
	void addEdgeIndex(int);
	int getEdgeIndexInFirstMotionless();
	void setEdgeIndexInFirstMotionless(int);
	int getEdgeIndexInSecondMotionless();
	void setEdgeIndexInSecondMotionless(int);
	std::vector<int>& getNeighbours();
	void addNeighbour(int);
	Geometry::Point& getRepresentivePoint();
	void setRepresentivePoint(Geometry::Point);
	int nodeIndex;
private:
	int firstMotionlessIndex;
	int secondMotionlessIndex;
	int edgeIndexInFirstMotionless;
	int edgeIndexInSecondMotionless;
	std::vector<int> edgeIndexes;
	std::vector<int> neighbours;
	Geometry::Point representivePoint;
};

#endif	/* _NODE_H */

