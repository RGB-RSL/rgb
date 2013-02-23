/*
 * File:   Circle.h
 * Author: Alimohammad Foroutannezhad
 * Modified by Eqbal Sarjami
 *
 * Created on September 17, 2010, 11:03 AM
 */

#ifndef CIRCLE_H
#define	CIRCLE_H

#include <iostream>
#include "Point.h"

namespace Geometry
{

	class Circle
	{
	public:

		Circle(Point C_center, double C_radius);

		Point getCenter(void);
		double getRadius(void);

		bool isInCircle(Point point);

	private:

		Point center;
		double radius;

	};
}

std::ostream & operator<<(std::ostream&, Geometry::Circle);

#endif	/* CIRCLE_H */