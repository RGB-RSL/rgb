/* 
 * File:   Vector.h
 * Author: Eqbal Sarjami
 *
 * Created on February 21, 2011, 5:00 PM
 */

#ifndef _VECTOR_H
#define	_VECTOR_H

#include "Point.h"


namespace Geometry
{
	class Point;

	class Vector
	{
	public:
		Vector();
		Vector(double x, double y);
		virtual ~Vector();

		void setX(double);
		void setY(double);
		double getX();
		double getY();

		void setFromPoint(Point);
		Point asPoint();
		double getLength();

		Vector operator +(Vector);
		Vector operator -(Vector);
		Vector operator *(double);
		bool operator<(Vector); //this function is just for sorting (and other STL utilities)
		bool operator ==(Vector);

	private:
		double x, y;
	};
	std::ostream & operator<<(std::ostream&, Vector);
}

#endif	/* _VECTOR_H */

