/* 
 * File:   AbstractChannel.cpp
 * Author: mehran
 * 
 * Created on October 6, 2010, 8:45 AM
 */

#include "AbstractChannel.h"
using namespace std;

AbstractChannel::AbstractChannel(string type, int num)
{
	this->type = type;
	this->channelNum = num;
}

AbstractChannel::~AbstractChannel()
{
}

bool AbstractChannel::operator ()(AbstractChannel c1, AbstractChannel c2)
{
	return c1.channelNum < c2.channelNum;
}

int AbstractChannel::getChannelNum() const
{
	return this->channelNum;
}

string AbstractChannel::getType() const
{
	return this->type;
}