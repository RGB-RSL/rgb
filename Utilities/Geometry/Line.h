/*
 * File:   Line.h
 * Author: Alimohammad Forutannezhad
 * Modified by Eqbal Sarjami
 *
 * Created on September 17, 2010, 11:03 AM
 */

#include <cmath>
#include <istream>
#include "Point.h"

#ifndef _LINE_H
#define _LINE_H

namespace Geometry
{
	class Segment;

	class Line
	{
	public:

		Line();
		Line(double m, double h);
		Line(Point, double degree);
		Line(Point, Point);
		~Line();

		void setM(double m);
		void setH(double h);
		void setHorizontal(bool isHorizontal = true);
		double getM();
		double getH();
		bool isHorizontal();

		double getX(double y);
		double getY(double x);
		Segment asSegment();

	private:
		double m, h;
		bool isHoriz;

		//if line is not horizontal line equation is y = mx + h
		//if line is horizontal line equation is x = h
	};
}
#endif
