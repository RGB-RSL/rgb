/*
 * File:   DoubleUtilities.cpp
 * Author: Alimohammad Forutannezhad
 * Modifed by Eqbal Sarjami
 *
 * Created on November 13, 2010, 12:49 PM
 */

#include "DoubleUtilities.h"
#include <cmath>

using namespace std;

const double EPS = 1e-8;

bool isEqual(double a, double b)
{
	return abs(a - b) < EPS;
}

bool isLess(double a, double b)
{
	return a + EPS < b;
}

bool isLessEqual(double a, double b)
{
	return a <= b + EPS;
}

bool hasSameSign(double a, double b)
{
	if(isEqual(a, 0.0) && isEqual(b, 0.0))
	{
		return true;
	}
	if(isEqual(a, 0.0) || isEqual(b, 0.0))
	{
		return false;
	}
	return a * b > 0.0;
}