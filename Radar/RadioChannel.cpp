/* 
 * File:   Channel.cpp
 * Author: mehran
 * 
 * Created on October 5, 2010, 8:20 PM
 */

#include "RadioChannel.h"
#include "Radar.h"
using namespace std;
using namespace Types;

RadioChannel::RadioChannel(): AbstractChannel("", -1)
{
}

RadioChannel::RadioChannel(string type, int channelNum, int bandWidth): AbstractChannel(type, channelNum)
{
	this->bandWidth = bandWidth;
}

RadioChannel::~RadioChannel()
{
}

void RadioChannel::setChannelJob(ChannelJob cj)
{
	this->channelJob = cj;
}

int RadioChannel::getBandWidth() const
{
	return this->bandWidth;
}

ChannelJob RadioChannel::getChannelJob() const
{
	return this->channelJob;
}