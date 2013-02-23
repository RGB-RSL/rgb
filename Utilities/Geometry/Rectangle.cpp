#include "Rectangle.h"
#include "../DoubleUtilities.h"
#include <cmath>

using namespace Geometry;

Rectangle::Rectangle(Point bl, Point tr)
{
	bottom_left = bl;
	top_right = tr;
}

double Rectangle::getLength()
{
	return (top_right.getX() - bottom_left.getX() > top_right.getY() - bottom_left.getY()) ?
	  top_right.getX() - bottom_left.getX() :
	  top_right.getY() - bottom_left.getY();
}

double Rectangle::getWidth()
{
	return (top_right.getX() - bottom_left.getX() > top_right.getY() - bottom_left.getY()) ?
	  top_right.getY() - bottom_left.getY() :
	  top_right.getX() - bottom_left.getX();
}

double Rectangle::getArea()
{
	return this->getLength() * this->getWidth();
}

bool Rectangle::isInRectangle(Point point)
{
	return isLessEqual(bottom_left.getX(), point.getX()) &&
	  isLessEqual(point.getX(), top_right.getX()) &&
	  isLessEqual(point.getY(), top_right.getY()) &&
	  isLessEqual(bottom_left.getY(), point.getY());
}

Point Rectangle::getCenter()
{
	return Point(((top_right.getX() - bottom_left.getX()) / 2), ((top_right.getY() - bottom_left.getY()) / 2));
}