/*
 * Bits.h
 *
 *  Created on: Jan 31, 2010
 *      Author: Hamed
 */

#include <vector>
#include <string>

#ifndef BITS_H_
#define BITS_H_

class bits
{
public:
	bits();
	bits(int, int);
	virtual ~bits();

	int getA();
	int getL();

	void setA(int);
	void setL(int);

private:
	int a, l;
};

//#include "bits.cpp"

#endif
