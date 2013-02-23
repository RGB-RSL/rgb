#include <errno.h>

#include "Message.h"
#include "../Utilities/Encodings.h"
#include "../Utilities/Debugger.h"
#include "TCPSocket.h"
#define LOGLEVEL 1

using namespace std;
using namespace Types;

Message::Message()
{
}

Message::~Message()
{
}

void Message::setAKConnect(Types::EntityType agentType, int tempID)
{
	message.clear();
	vector<byte> content = getAKConnectContent(agentType, tempID);
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_CONNECT).size() + content.size() + 12u);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_CONNECT));
	Encodings::writeUnsignedInt(message, content.size());
	Encodings::writeBytes(message, content);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKACKnowledge(int tempId, int id)
{
	message.clear();
	string header = Encodings::getMessageType(MT_AK_ACKNOWLEDGE);
	Encodings::writeUnsignedInt(message, header.size() + 20);
	Encodings::writeString(message, header);
	Encodings::writeUnsignedInt(message, 8);
	Encodings::writeUnsignedInt(message, tempId);
	Encodings::writeUnsignedInt(message, id);
	Encodings::writeUnsignedInt(message, 0u);
}

vector<byte> Message::getAKConnectContent(Types::EntityType agentType, int tempID)
{
	vector<byte> b;

	Encodings::writeUnsignedInt(b, tempID);
	Encodings::writeUnsignedInt(b, 1u);
	Encodings::writeString(b, "Eskilas.PoliceForce11111");
	Encodings::writeUnsignedInt(b, 1u);
	Encodings::writeString(b, Encodings::getEntityType(agentType));

	return b;
}
//////////////////////////////

void Message::setAKClear(int selfID, int time, int targetID)
{
	message.clear();
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Clear).size() + 24);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Clear));
	Encodings::writeUnsignedInt(message, 12);
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeUnsignedInt(message, targetID);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKExtinguish(int selfID, int time, int targetID, int water)
{
	message.clear();
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Extinguish).size() + 28);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Extinguish));
	Encodings::writeUnsignedInt(message, 16);
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeUnsignedInt(message, targetID);
	Encodings::writeUnsignedInt(message, water);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKLoad(int selfID, int time, int targetID)
{
	message.clear();
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Load).size() + 24);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Load));
	Encodings::writeUnsignedInt(message, 12);
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeUnsignedInt(message, targetID);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKMove(int selfID, int time, vector<int> &path, int destinationX, int destinationY)
{
	message.clear();
	int size = path.size() * 4;
	size += 32;
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Move).size() + size);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Move));
	Encodings::writeUnsignedInt(message, 20 + path.size() * 4);
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeUnsignedInt(message, path.size());
	for(int i = 0; i < (int)path.size(); i++)
		Encodings::writeUnsignedInt(message, path[i]);
	Encodings::writeUnsignedInt(message, destinationX);
	Encodings::writeUnsignedInt(message, destinationY);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKRescue(int selfID, int time, int targetID)
{
	message.clear();
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Rescue).size() + 24);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Rescue));
	Encodings::writeUnsignedInt(message, 12);
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeUnsignedInt(message, targetID);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKRest(int selfID, int time)
{
	message.clear();
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Rest).size() + 20);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Rest));
	Encodings::writeUnsignedInt(message, 8);
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKSay(int selfID, int time, string saymessage)
{
	message.clear();
	int size = saymessage.size() + 4 * 4;
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Say).size() + size);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Say));
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeString(message, saymessage);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKSpeak(int selfID, int time, int channel, vector<byte> speakmessage)
{
	message.clear();
	int size = speakmessage.size() + 28;
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Speak).size() + size);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Speak));
	Encodings::writeUnsignedInt(message, speakmessage.size() + 16);
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeUnsignedInt(message, channel);
	Encodings::writeUnsignedInt(message, speakmessage.size());
	Encodings::writeBytes(message, speakmessage);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKSubscribe(int selfID, int time, vector<int> channels)
{
	message.clear();
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Subscribe).size() + 24 + channels.size() * 4);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Subscribe));
	Encodings::writeUnsignedInt(message, channels.size() * 4 + 12);
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeUnsignedInt(message, channels.size());
	for(int i = 0; i < (int)channels.size(); i++)
		Encodings::writeUnsignedInt(message, channels[i]);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKTell(int selfID, int time, string tellmessage)
{
	message.clear();
	int size = tellmessage.size() + 4 * 4;
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Tell).size() + size);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Tell));
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeString(message, tellmessage);
	Encodings::writeUnsignedInt(message, 0u);
}

void Message::setAKUnload(int selfID, int time)
{
	message.clear();
	Encodings::writeUnsignedInt(message, Encodings::getMessageType(MT_AK_Unload).size() + 20);
	Encodings::writeString(message, Encodings::getMessageType(MT_AK_Unload));
	Encodings::writeUnsignedInt(message, 8);
	Encodings::writeUnsignedInt(message, selfID);
	Encodings::writeUnsignedInt(message, time);
	Encodings::writeUnsignedInt(message, 0u);
}

vector<byte> Message::getMessage()
{
	return message;
}
