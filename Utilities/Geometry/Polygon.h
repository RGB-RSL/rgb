/*
 * File:   Polygon.h
 * Author: Hamed Saleh
 *
 * Created on November 1, 2010, 7:03 PM
 */

#include <iostream>
#include <vector>
#include "Point.h"

#ifndef _POLYGON_H
#define	_POLYGON_H

namespace Geometry
{
    class Segment;

    class Polygon
    {
    public:
        Polygon();
        Polygon(std::vector <Point>);
        virtual ~Polygon();
        int size();
        void setVertices(std::vector <Point>);
        void addVertex(Point);
        std::vector <Point>& getVertices();
        void clear();
        Point getVertex(int);
        Segment getSegment(int);

		Point getMidPoint();
		bool isInPolygon(Point);

    private:
        std::vector<Point> vertices;
    };
}

std::ostream & operator<<(std::ostream&, Geometry::Polygon);

#endif
