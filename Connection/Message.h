/* 
 * File:   Message.h
 * Author: hellibash
 *
 * Created on August 9, 2010, 11:22 AM
 */

#ifndef _MESSAGE_H
#define	_MESSAGE_H

#include <vector>
#include "../Utilities/Types/Types.h"

class Message
{
public:
	Message();
	virtual ~Message();

	void setAKConnect(Types::EntityType agentType, int tempID);
	void setAKACKnowledge(int tempId, int id);
	std::vector<byte> getMessage();

	void setAKClear(int selfID, int time, int targetID);
	void setAKExtinguish(int selfID, int time, int targetID, int water);
	void setAKLoad(int selfID, int time, int targetID);
	void setAKMove(int selfID, int time, std::vector<int> &path, int destinationX, int destinationY);
	void setAKRescue(int selfID, int time, int targetID);
	void setAKRest(int selfID, int time);
	void setAKSay(int selfID, int time, std::string saymessage);
	void setAKSpeak(int selfID, int time, int channel, std::vector<byte> speakmessage);
	void setAKSubscribe(int selfID, int time, std::vector<int> channel);
	void setAKTell(int selfID, int time, std::string tellmessage);
	void setAKUnload(int selfID, int time);

private:
	std::vector<byte> message;

	std::vector<byte> getAKConnectContent(Types::EntityType agentType, int tempID);
};


#endif	/* _MESSAGE_H */

