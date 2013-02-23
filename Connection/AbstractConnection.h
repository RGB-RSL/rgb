/*
 * AbstractConnection.cpp
 *
 *  Created on: Dec 28, 2010
 *      Author: Mehran Akhavan
 */
#ifndef _ABSTRACTCONNECTION_H
#define	_ABSTRACTCONNECTION_H

#include "../Utilities/Types/Types.h"
#include <iostream>
#include <vector>
#include <fstream>

class AbstractConnection
{
public:
	virtual bool init() = 0;
	virtual bool sendMessage(const std::vector<byte> &msg) = 0;
	virtual bool getMessage(std::vector<byte> &msg) = 0;
	virtual void done() = 0;
};

#endif