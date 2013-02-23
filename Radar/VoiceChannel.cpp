/* 
 * File:   VoiceChannel.cpp
 * Author: mehran
 * 
 * Created on October 5, 2010, 10:14 PM
 */

#include "VoiceChannel.h"
#define LOGLEVEL 1
using namespace std;

VoiceChannel::VoiceChannel(): AbstractChannel("", -1)
{
}

VoiceChannel::VoiceChannel(std::string type, int channelNum, int maxSize, int maxNumOfMessage, int range): AbstractChannel(type, channelNum)
{
	this->maxSize = maxSize;
	this->maxNumOfMessage = maxNumOfMessage;
	this->range = range;
}

VoiceChannel::~VoiceChannel()
{
}

int VoiceChannel::getMaxNumOfMessage() const
{
	return this->maxNumOfMessage;
}

int VoiceChannel::getMaxSize() const
{
	return this->maxSize;
}

int VoiceChannel::getRange() const
{
	return this->range;
}