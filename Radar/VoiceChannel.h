/* 
 * File:   VoiceChannel.h
 * Author: mehran
 *
 * Created on October 5, 2010, 10:14 PM
 */

#ifndef _VOICECHANNEL_H
#define	_VOICECHANNEL_H
#include <iostream>
#include <string>
#include "../Utilities/Debugger.h"
#include "AbstractChannel.h"

class VoiceChannel: public AbstractChannel
{
public:
	VoiceChannel();
	VoiceChannel(std::string type, int channelNum, int maxSize, int maxNumOfMessage, int range);
	virtual ~VoiceChannel();
	int getRange() const;
	int getMaxSize() const;
	int getMaxNumOfMessage() const;
private:
	int maxSize;
	int maxNumOfMessage;
	int range;
};

#endif	/* _VOICECHANNEL_H */

