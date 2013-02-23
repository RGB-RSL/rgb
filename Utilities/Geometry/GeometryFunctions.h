/* 
 * File:   GeometryFunctions.h
 * Author: Eqbal Sarjami
 *
 * Created on January 3, 2011, 9:28 PM
 */

#ifndef _GEOMETRYFUNCTIONS_H
#define	_GEOMETRYFUNCTIONS_H
#include "Point.h"
#include "Segment.h"
#include "Polygon.h"

namespace Geometry
{
    double distanceBetweenPoints(Point, Point);
    double crossProduct(Vector, Vector);
    double dotProduct(Vector, Vector);
    bool isParallel(Segment, Segment);
    bool isParallel(Line, Line);
    double distanceToLine(Line, Point);
    double distanceToSegment(Segment, Point);
    bool isInSameSide(Segment, Point, Point);
    bool isInSameSide(Line, Point, Point);
    bool isIntersect(Segment, Segment);
    bool isIntersect(Line, Segment);
    Point getIntersectPoint(Line, Line);
	Line getPerpendicularLine(Line, Point);
	Point getPerpendicularPoint(Line, Point);
	Line getParallelLine(Line, Point);
	bool isOnSegment(Segment, Point);
	Vector getVectorBylength(Vector, double length);
	Polygon increasePolygon(Polygon, double d); //it'l decrease polygon if d is less than 0

    void setColor(int);
    extern int __color;

    //for use angleCompare you must set compare point by setComparePoint function
    //all the points that wants to be sorted by this compare must be on one side of the compare point (else it can cause runtime errors)
    bool angleCompare(Point, Point);
    void setComparePoint(Point);
    extern Point __comparePoint;
}

#endif	/* _GEOMETRYFUNCTIONS_H */

