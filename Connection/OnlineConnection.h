
#ifndef __CONNECTION_H_
#define __CONNECTION_H_

#include "TCPSocket.h"
#include "AbstractConnection.h"
#include <string>
#include <vector>

typedef unsigned char byte;

//#include <zeitgeist/node.h> // #include <zeitgeist/leaf.h> error: expected constructor...

class OnlineConnection: public AbstractConnection //:public zeitgeist::Leaf
{
protected:
	std::string mHost;
	unsigned mPort;

	bool isDelete;

	bool selectInput();

public:
	TCPSocket mSocket;

public:
	OnlineConnection(const std::string& = "127.0.0.1", unsigned = 3100);
	virtual ~OnlineConnection();

	static OnlineConnection * instance();

	bool init();
	bool sendMessage(const std::vector<byte> &msg);
	bool getMessage(std::vector<byte> &msg);

	//	TCPSocket getSocket();

	void done();
};

//DECLARE_CLASS(Connection);

#endif //__CONNECTION_H_
