/* 
 * File:   OfflineConnection.h
 * Author: mehran
 *
 * Created on September 28, 2010, 2:21 PM
 */

#ifndef _OFFLINECONNECTION_H
#define	_OFFLINECONNECTION_H
#include "AbstractConnection.h"
#include <string>

class OfflineConnection: public AbstractConnection
{
public:
	OfflineConnection(std::string fileDir);
	virtual ~OfflineConnection();
	bool init();
	bool getMessage(std::vector<byte> &msg);
	bool sendMessage(const std::vector<byte> &msg);
	void done();
private:
	std::ifstream in;

};

#endif	/* _OFFLINECONNECTION_H */

