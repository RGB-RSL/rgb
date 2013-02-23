/*
 * Bits.cpp
 *
 *  Created on: Jan 31, 2010
 *      Author: Hamed
 */

#include "BoolStream.h"
#include <iostream>
#include <string>

using namespace std;

bits::bits()
{
}

bits::~bits()
{
}

bits::bits(int a, int l)
{
	this -> a = a;
	this -> l = l;
}

int bits::getA()
{
	return a;
}

void bits::setA(int a)
{
	this -> a = a;
}

int bits::getL()
{
	return l;
}

void bits::setL(int l)
{
	this -> l = l;
}
