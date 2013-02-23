/*
 * File:   Point.h
 * 
 * Modifed by Eqbal Sarjami
 * on February 21, 2011
 */

#ifndef __POINT_H_
#define __POINT_H_
#include <cmath>
#include <fstream>

#include "Vector.h"

namespace Geometry
{
	class Vector;

	class Point
	{
	public:
		Point();
		Point(double x, double y);
		virtual ~Point();

		void setX(double);
		void setY(double);
		double getX();
		double getY();

		void setFromVector(Vector);
		Vector asVector();

		bool operator<(Point); //this function is just for sorting (and other STL utilities)
		bool operator ==(Point);
		double distFromPoint(Point point);

	private:
		double x, y;
	};
}

std::ostream & operator<<(std::ostream&, Geometry::Point);

#endif
