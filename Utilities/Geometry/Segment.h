/* 
 * File:   Segment.h
 * Author: Alimohammad Foroutannezhad
 * Modified by Eqbal Sarjami
 *
 * Created on September 17, 2010, 11:03 AM
 */

#ifndef SEGMENT_H
#define	SEGMENT_H

#include <cmath>
#include <Point.h>
#include <iostream>
#include <Line.h>
#include <../DoubleUtilities.h>

namespace Geometry
{

	class Segment
	{
	public:

		Segment();
		Segment(Point a, Point b);
		~Segment();

		void setFirstPoint(Point);
		void setSecondPoint(Point);
		Point getFirstPoint();
		Point getSecondPoint();

		double getLength();
		Point getMiddlePoint();
		Vector asVector();
		Vector asRVector();
		Line asLine();
		
	private:
		Point first;
		Point second;
	};
}

std::ostream & operator<<(std::ostream&, Geometry::Segment);

#endif	/* SEGMENT_H */

