/* 
 * File:   AbstractChannel.h
 * Author: mehran
 *
 * Created on October 6, 2010, 8:45 AM
 */

#ifndef _ABSTRACTCHANNEL_H
#define	_ABSTRACTCHANNEL_H
#include <iostream>
#include <string>

class AbstractChannel
{
public:
	AbstractChannel(std::string type, int num);
	virtual ~AbstractChannel();
	bool operator() (AbstractChannel c1, AbstractChannel c2);
	std::string getType() const;
	int getChannelNum() const;
protected:
	int channelNum;
	std::string type;

};


#endif	/* _ABSTRACTCHANNEL_H */

