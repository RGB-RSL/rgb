/* 
 * File:   Parser.h
 * Author: eqbal
 *
 * Created on November 16, 2010, 3:51 PM
 */

#ifndef _PARSER_H
#define	_PARSER_H

#include <vector>
#include <string>
#include "../Utilities/Types/Types.h"
#include "WorldModel.h"

namespace Parser
{
	std::string analyzeMessage(WorldModel* world, std::vector<byte> &msg);
	std::string parseKAConnectOk(WorldModel* world, std::vector<byte> &msg, int &offset);
	std::string parseKAConnectError(WorldModel* world, std::vector<byte> &msg, int &offset);
	std::string parseKASense(WorldModel* world, std::vector<byte> &msg, int &offset);
	std::string parseKAHear(WorldModel* world, std::vector<byte> &msg, int &offset);
	void parseObjects(WorldModel* world, std::vector<byte> &message, RCRObject &object, int &offset, int counter);
	void parseObjectElements(WorldModel* world, std::vector<byte> &message, int &offset, std::string type, int id, int counter);
	void parseObjectProperties(WorldModel* world, std::vector<byte> &message, int &offset);
	void parseObjectsElementsKAConnectOk(WorldModel* world, std::vector<byte> &message, int &offset);
	void parseObjectsElementsKASense(WorldModel* world, std::vector<byte> &message, int &offset);
	void parseRadarMessages(WorldModel* world, std::vector<byte> &message, int &offset);
	void parseConfig(WorldModel* world, std::vector<byte> &message, int &offset);
}

#endif	/* _PARSER_H */

