/* 
 * File:   Encodings.h
 * Author: hellibash
 *
 * Created on August 9, 2010, 11:25 AM
 */

#ifndef _ENCODINGS_H
#define	_ENCODINGS_H

#include <string>
#include <vector>
#include "Types/Types.h"

namespace Encodings
{
	std::string getEntityType(Types::EntityType agentType);
	std::string getMessageType(Types::MessageType messageType);
	std::string getPropertyType(Types::PropertyType propertyType);
	std::string getConfigType(Types::ConfigType configType);
	void writeUnsignedInt(std::vector<byte> &b, unsigned int v);
	void writeByte(std::vector<byte> &b, byte v);
	void writeString(std::vector<byte> &b, std::string v);
	void writeRadarString(std::vector<byte> &b, std::string v);
	void writeBytes(std::vector<byte> &b, const std::vector<byte> v);
	bool readBool(std::vector<byte>& in, int &offset);
	unsigned int readInt(std::vector<byte>& in, int &offset);
	std::string readString(std::vector<byte> &in, int &offset);
	byte readByte(std::vector<byte> &in, int &offset);
	std::string readRadarString(std::vector<byte> &in, int &offset, int len);
}

#endif	/* _ENCODINGS_H */

