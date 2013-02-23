/* 
 * File:   Rectangle.h
 * Author: Alimohammad
 *
 * Created on September 17, 2010, 2:12 PM
 */

#ifndef RECTANGLE_H
#define	RECTANGLE_H
#include "Point.h"
#include <iostream>

namespace Geometry
{

	class Rectangle
	{
	private:
		Point bottom_left;
		Point top_right;

	public:
		Rectangle(Point bottom_left, Point top_right);

		double getLength();
		double getWidth();
		double getArea();
		bool isInRectangle(Point point);

		Point getCenter();
	};
}

#endif	/* RECTANGLE_H */

