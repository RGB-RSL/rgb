#include <vector>
#include <iostream>
#include <string.h>
#include <netinet/in.h>
#include <sstream>
#include <errno.h>
#include "Encodings.h"
using namespace std;
using namespace Types;

string Encodings::getEntityType(Types::EntityType agentType)
{
	switch(agentType)
	{
		case ET_FIRE_BRIGADE:
			return "urn:rescuecore2.standard:entity:firebrigade";
		case ET_FIRE_STATION:
			return "urn:rescuecore2.standard:entity:firestation";
		case ET_AMBULANCE_TEAM:
			return "urn:rescuecore2.standard:entity:ambulanceteam";
		case ET_AMBULANCE_CENTER:
			return "urn:rescuecore2.standard:entity:ambulancecentre";
		case ET_POLICE_FORCE:
			return "urn:rescuecore2.standard:entity:policeforce";
		case ET_POLICE_OFFICE:
			return "urn:rescuecore2.standard:entity:policeoffice";
		case ET_BULDING:
			return "urn:rescuecore2.standard:entity:building";
		case ET_REFUGE:
			return "urn:rescuecore2.standard:entity:refuge";
		case ET_ROAD:
			return "urn:rescuecore2.standard:entity:road";
		case ET_BLOCKADE:
			return "urn:rescuecore2.standard:entity:blockade";
		case ET_CIVILIAN:
			return "urn:rescuecore2.standard:entity:civilian";
		case ET_WORLD:
			return "urn:rescuecore2.standard:entity:world";
		default:
			return "";
	}

}

string Encodings::getMessageType(Types::MessageType messageType)
{
	switch(messageType)
	{
		case MT_AK_CONNECT:
			return "urn:rescuecore2:messages.control:ak_connect";
		case MT_AK_ACKNOWLEDGE:
			return "urn:rescuecore2:messages.control:ak_acknowledge";
		case MT_KA_CONNECT_OK:
			return "urn:rescuecore2:messages.control:ka_connect_ok";
		case MT_KA_CONNECT_ERROR:
			return "urn:rescuecore2:messages.control:ka_connect_error";
		case MT_KA_SENSE:
			return "urn:rescuecore2:messages.control:ka_sense";
		case MT_KA_HEAR:
			return "urn:rescuecore2:messages.control:ka_hear";
		case MT_AK_Rest:
			return "urn:rescuecore2.standard:message:rest";
		case MT_AK_Move:
			return "urn:rescuecore2.standard:message:move";
		case MT_AK_Load:
			return "urn:rescuecore2.standard:message:load";
		case MT_AK_Unload:
			return "urn:rescuecore2.standard:message:unload";
		case MT_AK_Say:
			return "urn:rescuecore2.standard:message:say";
		case MT_AK_Tell:
			return "urn:rescuecore2.standard:message:tell";
		case MT_AK_Extinguish:
			return "urn:rescuecore2.standard:message:extinguish";
		case MT_AK_Rescue:
			return "urn:rescuecore2.standard:message:rescue";
		case MT_AK_Clear:
			return "urn:rescuecore2.standard:message:clear";
		case MT_AK_Subscribe:
			return "urn:rescuecore2.standard:message:subscribe";
		case MT_AK_Speak:
			return "urn:rescuecore2.standard:message:speak";
		default:
			return "";
	}
}

string Encodings::getPropertyType(Types::PropertyType propertyType)
{
	switch(propertyType)
	{
		case PT_APEXES:
			return "urn:rescuecore2.standard:property:apexes";
		case PT_BLOCKADES:
			return "urn:rescuecore2.standard:property:blockades";
		case PT_BROKENNESS:
			return "urn:rescuecore2.standard:property:brokenness";
		case PT_BUILDING_AREA_GROUND:
			return "urn:rescuecore2.standard:property:buildingareaground";
		case PT_BUILDING_AREA_TOTAL:
			return "urn:rescuecore2.standard:property:buildingareatotal";
		case PT_BUILDING_ATTRIBUTES:
			return "urn:rescuecore2.standard:property:buildingattributes";
		case PT_BUILDING_CODE:
			return "urn:rescuecore2.standard:property:buildingcode";
		case PT_BURIEDNESS:
			return "urn:rescuecore2.standard:property:buriedness";
		case PT_DAMAGE:
			return "urn:rescuecore2.standard:property:damage";
		case PT_DIRECTION:
			return "urn:rescuecore2.standard:property:direction";
		case PT_EDGES:
			return "urn:rescuecore2.standard:property:edges";
		case PT_FIERYNESS:
			return "urn:rescuecore2.standard:property:fieryness";
		case PT_FLOORS:
			return "urn:rescuecore2.standard:property:floors";
		case PT_HP:
			return "urn:rescuecore2.standard:property:hp";
		case PT_IGNITION:
			return "urn:rescuecore2.standard:property:ignition";
		case PT_IMPORTANCE:
			return "urn:rescuecore2.standard:property:importance";
		case PT_LATITUDE:
			return "urn:rescuecore2.standard:property:latitude";
		case PT_LONGTITUDE:
			return "urn:rescuecore2.standard:property:longtitude";
		case PT_POSITION:
			return "urn:rescuecore2.standard:property:position";
		case PT_POSITION_HISTORY:
			return "urn:rescuecore2.standard:property:positionhistory";
		case PT_REPAIR_COST:
			return "urn:rescuecore2.standard:property:repaircost";
		case PT_STAMINA:
			return "urn:rescuecore2.standard:property:stamina";
		case PT_START_TIME:
			return "urn:rescuecore2.standard:property:starttime";
		case PT_TEMPERATURE:
			return "urn:rescuecore2.standard:property:temperature";
		case PT_TRAVEL_DISTANCE:
			return "urn:rescuecore2.standard:property:traveldistance";
		case PT_WATER_QUANTITY:
			return "urn:rescuecore2.standard:property:waterquantity";
		case PT_WIND_DIRECTION:
			return "urn:rescuecore2.standard:property:winddirection";
		case PT_WIND_FORCE:
			return "urn:rescuecore2.standard:property:windforce";
		case PT_X:
			return "urn:rescuecore2.standard:property:x";
		case PT_Y:
			return "urn:rescuecore2.standard:property:y";
		default:
			return "";
	}
}

string Encodings::getConfigType(Types::ConfigType configType)
{
	switch(configType)
	{
		case CT_NUM_OF_CHANNELS:
			return "comms.channels.count";
		case CT_BANDWIDTH_SUFIX:
			return ".bandwidth";
		case CT_CENTER_MAX_CHANNELS:
			return "comms.channels.max.centre";
		case CT_COMMUNICATION_PREFIX:
			return "comms.channels.";
		case CT_NOISE_SUFFIX:
			return ".noise";
		case CT_NOISE_TYPE_DROPOUT:
			return "dropout";
		case CT_NOISE_TYPE_STATIC:
			return "static";
		case CT_PLATOON_MAX_CHANNELS:
			return "comms.channels.max.platoon";
		case CT_VOICE_RANGE:
			return ".range";
		case CT_TYPE_RADAR:
			return "radio";
		case CT_TYPE_SUFFIX:
			return ".type";
		case CT_TYPE_VOICE:
			return "voice";
		case CT_MESSAGE_MAX_SUFFIX:
			return ".messages.max";
		case CT_MESSAGE_SIZE_SUFFIX:
			return ".messages.size";
		case CT_IGNORE_UNTIL:
			return "kernel.agents.ignoreuntil";
		case CT_THINK_TIME:
			return "kernel.agents.think-time";
		case CT_STARTUP_CONNECT_TIME:
			return "kernel.startup.connect-time";
        case CT_DAMAGE_PERCEPTION:
            return "perception.los.precision.damage";
        case CT_HP_PERCEPTION:
            return "perception.los.precision.hp";
        case CT_REPAIR_RATE:
            return "clear.repair.rate";
        case CT_CLEAR_DISTANCE:
            return "clear.repair.distance";
        case CT_SIHGT_RANGE:
            return "perception.los.max-distance";
		default:
			return "";
	}
}

void Encodings::writeByte(vector<byte>& b, byte v)
{
	b.push_back(v);
}

void Encodings::writeUnsignedInt(vector<byte>& b, unsigned int v)
{
	b.push_back((v >> 24) & 0xFF);
	b.push_back((v >> 16) & 0xFF);
	b.push_back((v >> 8) & 0xFF);
	b.push_back(v & 0xFF);
}

void Encodings::writeString(vector<byte>& b, string v)
{
	writeUnsignedInt(b, v.length());
	unsigned int s = b.size();
	b.resize(s + v.length(), 0);
	for(unsigned int i = 0; i < v.length(); i++)
		b[s + i] = v[i];
}

void Encodings::writeRadarString(vector<byte>& b, string v)
{
	unsigned int s = b.size();
	b.resize(s + v.length(), 0);
	for(unsigned int i = 0; i < v.length(); i++)
		b[s + i] = v[i];
}

void Encodings::writeBytes(vector<byte>& b, const vector<byte> v)
{
	unsigned int s = b.size();
	b.resize(s + v.size(), 0);
	for(unsigned int i = 0; i < v.size(); i++)
		b[s + i] = v[i];
}

bool Encodings::readBool(vector<byte>& in, int& offset)
{
	int b = in[offset];
	offset++;
	return b == 1;
}

unsigned int Encodings::readInt(vector<byte>& in, int &offset)
{
	unsigned int value = ntohl(*((unsigned int*)(&in[0] + offset)));
	offset += 4;
	return value;

}

byte Encodings::readByte(vector<byte>& in, int& offset)
{
	byte b = in[offset];
	offset++;
	return b;
}

string Encodings::readString(vector<byte>& in, int &offset)
{
	int len = readInt(in, offset);
	//    char* buffer[len];
	string ret = "";
	ret.resize(len);
	//    memcpy(&buffer, &in[offset], len);
	for(int i = 0; i < len; i++)
		ret[i] = in[i + offset];
	offset += len;
	return ret;
}

string Encodings::readRadarString(vector<byte>& in, int& offset, int len)
{
	//    char* buffer[len];
	string ret = "";
	ret.resize(len);
	//    memcpy(&buffer, &in[offset], len);
	for(int i = 0; i < len; i++)
		ret[i] = in[i + offset];
	offset += len;
	return ret;
}
