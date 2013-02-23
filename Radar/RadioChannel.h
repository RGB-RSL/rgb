/* 
 * File:   Channel.h
 * Author: mehran
 *
 * Created on October 5, 2010, 8:20 PM
 */

#ifndef _CHANNEL_H
#define	_CHANNEL_H
#include <iostream>
#include <string>
#include "../Utilities/Types/Types.h"
#include "AbstractChannel.h"

class RadioChannel: public AbstractChannel
{
public:
	RadioChannel();
	RadioChannel(std::string type, int channelNum, int bandWidth);
	virtual ~RadioChannel();
	int getBandWidth() const;
	Types::ChannelJob getChannelJob() const;
	void setChannelJob(Types::ChannelJob cj);
	//    int getDropoutNoise() const;
	//    int getFailureNoise() const;
private:
	int bandWidth;
	Types::ChannelJob channelJob;

	//    int dropoutNoise;
	//    int failureNoise;

};

#endif	/* _CHANNEL_H */

