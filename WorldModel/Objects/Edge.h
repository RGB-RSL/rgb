/* 
 * File:   Edge.h
 * Author: Eqbal Sarjami
 *
 * Created on November 28, 2010, 1:40 PM
 */

#include "../../Utilities/Types/Types.h"
#include "MotionlessObject.h"

#ifndef _EDGE_H
#define	_EDGE_H

class Edge
{
public:
	Edge(int, int);
	~Edge();
	int getFirstNodeIndex();
	int getSecondNodeIndex();
	int getLength();
	int getMotionlessIndex();
	void setLength(int);
	Types::PassingMode getPassingMode();
	void setPassingMode(Types::PassingMode);
	void setMotionlessIndex(int motionlessIndex);
	int edgeIndex;
private:
	int firstNodeIndex;
	int secondNodeIndex;
	int length;
	Types::PassingMode passingMode;
	int motionlessIndex;
};

#endif	/* _EDGE_H */

